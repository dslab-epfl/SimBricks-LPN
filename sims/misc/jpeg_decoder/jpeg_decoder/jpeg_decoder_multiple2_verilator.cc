
/*
 * Copyright 2023 Max Planck Institute for Software Systems, and
 * National University of Singapore
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "include/jpeg_decoder_multiple2_verilator.hh"

#include <signal.h>
#include <string.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <type_traits>

#include "include/jpeg_decoder_regs.hh"
#include "simbricks/base/cxxatomicfix.h"
extern "C" {
#include "simbricks/pcie/if.h"
}

#include "simbricks/pcie/proto.h"

#define DEBUG 0

static VerilatedContext vcontext{};
static Vjpeg_decoder_multiple2 top{"jpeg_decoder_multiple2"};
static VerilatedVcdC trace{};

static JpegDecoderMemReader jpeg_decoder_mem_reader{top};
static JpegDecoderMemWriter jpeg_decoder_mem_writer{top};
static JpegDecoderMMIOInterface jpeg_decoder_mmio{top, mmio_done};
static ClockGaterMMIOInterface clock_gater_mmio{top, mmio_done};

static uint64_t clock_period = 1'000'000 / 150ULL;  // 150 MHz
static int exiting;
static int print_cur_ts_requested;
static bool tracing_active;
static char *trace_filename;
static uint64_t trace_nr;
static uint64_t tracing_start;  // used to make every trace start at 0
static struct SimbricksPcieIf pcieif;

// Expose control over Verilator simulation through BAR. This represents the
// underlying memory that's being accessed.
VerilatorRegs verilator_regs{};

extern "C" void sigint_handler(int dummy) {
  exiting = 1;
}

extern "C" void sigusr1_handler(int dummy) {
  print_cur_ts_requested = 1;
}

volatile union SimbricksProtoPcieD2H *d2h_alloc(uint64_t cur_ts) {
  volatile union SimbricksProtoPcieD2H *msg;
  while (!(msg = SimbricksPcieIfD2HOutAlloc(&pcieif, cur_ts))) {
  }
  return msg;
}

bool PciIfInit(const char *shm_path,
               struct SimbricksBaseIfParams &baseif_params) {
  struct SimbricksBaseIfSHMPool pool;
  struct SimBricksBaseIfEstablishData ests;
  struct SimbricksProtoPcieDevIntro d_intro;
  struct SimbricksProtoPcieHostIntro h_intro;

  memset(&pool, 0, sizeof(pool));
  memset(&ests, 0, sizeof(ests));
  memset(&d_intro, 0, sizeof(d_intro));

  d_intro.pci_vendor_id = 0xdead;
  d_intro.pci_device_id = 0xbeef;
  d_intro.pci_class = 0x40;
  d_intro.pci_subclass = 0x00;
  d_intro.pci_revision = 0x00;

  static_assert(sizeof(VerilatorRegs) <= 4096, "Registers don't fit BAR");
  d_intro.bars[0].len = 4096;
  d_intro.bars[0].flags = 0;
  static_assert(sizeof(JpegDecoderRegs) <= 4096, "Registers don't fit BAR");
  d_intro.bars[1].len = 4096;
  d_intro.bars[1].flags = 0;
  static_assert(sizeof(ClockGaterRegs) <= 4096, "Registers don't fit BAR");
  d_intro.bars[2].len = 4096;
  d_intro.bars[2].flags = 0;

  ests.base_if = &pcieif.base;
  ests.tx_intro = &d_intro;
  ests.tx_intro_len = sizeof(d_intro);
  ests.rx_intro = &h_intro;
  ests.rx_intro_len = sizeof(h_intro);

  if (SimbricksBaseIfInit(&pcieif.base, &baseif_params)) {
    std::cerr << "PciIfInit: SimbricksBaseIfInit failed\n";
    return false;
  }

  if (SimbricksBaseIfSHMPoolCreate(
          &pool, shm_path, SimbricksBaseIfSHMSize(&pcieif.base.params)) != 0) {
    std::cerr << "PciIfInit: SimbricksBaseIfSHMPoolCreate failed\n";
    return false;
  }

  if (SimbricksBaseIfListen(&pcieif.base, &pool) != 0) {
    std::cerr << "PciIfInit: SimbricksBaseIfListen failed\n";
    return false;
  }

  if (SimBricksBaseIfEstablish(&ests, 1)) {
    std::cerr << "PciIfInit: SimBricksBaseIfEstablish failed\n";
    return false;
  }

  return true;
}

// react to changes of ctrl signals
void apply_ctrl_changes() {
  // change to tracing_active
  if (!tracing_active && verilator_regs.tracing_active) {
    tracing_active = true;
    // TODO(Kaufi-Jonas): this doesn't work at the moment
    // tracing_start = vcontext.time();
    std::ostringstream trace_file{};
    trace_file << std::string{trace_filename} << "_" << trace_nr << ".vcd";
    std::cout << "trace_file: " << trace_file.str() << "\n";

    top.trace(&trace, 0);
    trace.open(trace_file.str().c_str());
    ++trace_nr;
  } else if (tracing_active && !verilator_regs.tracing_active) {
    tracing_active = false;
    trace.close();
  }
}

bool h2d_read(volatile struct SimbricksProtoPcieH2DRead &read,
              uint64_t cur_ts) {
  switch (read.bar) {
    case 0: {
      if (read.offset + read.len > sizeof(verilator_regs)) {
        std::cerr
            << "error: read from verilator registers outside bounds offset="
            << read.offset << " len=" << read.len << "\n";
        return false;
      }

      volatile union SimbricksProtoPcieD2H *msg = d2h_alloc(cur_ts);
      volatile struct SimbricksProtoPcieD2HReadcomp *readcomp = &msg->readcomp;
      readcomp->req_id = read.req_id;
      memcpy((void *)readcomp->data, (uint8_t *)&verilator_regs + read.offset,
             read.len);
      SimbricksPcieIfD2HOutSend(&pcieif, msg,
                                SIMBRICKS_PROTO_PCIE_D2H_MSG_READCOMP);

      break;
    }
    case 1: {
      jpeg_decoder_mmio.issueRead(read.req_id, read.offset, read.len);
      break;
    }
    case 2: {
      clock_gater_mmio.issueRead(read.req_id, read.offset, read.len);
      break;
    }
    default: {
      std::cerr << "error: read from unexpected bar " << read.bar << "\n";
      return false;
    }
  }
  return true;
}

bool h2d_write(volatile struct SimbricksProtoPcieH2DWrite &write,
               uint64_t cur_ts, bool posted) {
#if DEBUG
  std::cout << "h2d_write bar=" << (int)write.bar << " offset=" << write.offset
            << " len=" << write.len << std::endl;
#endif

  uint64_t val = 0;
  memcpy(&val, (void *)write.data, std::min<size_t>(write.len, (sizeof(val))));

  switch (write.bar) {
    case 0:
      if (write.offset + write.len > sizeof(verilator_regs)) {
        std::cerr
            << "error: write to verilator registers outside bounds offset="
            << write.offset << " len=" << write.len << "\n";
        return false;
      }
      memcpy(((uint8_t *)&verilator_regs) + write.offset, (void *)write.data,
             write.len);
      apply_ctrl_changes();
      break;
    case 1:
      jpeg_decoder_mmio.issueWrite(write.req_id, write.offset, write.len, val);
      break;
    case 2:
      clock_gater_mmio.issueWrite(write.req_id, write.offset, write.len, val);
      break;
    default:
      std::cerr << "error: write to unexpected bar " << write.bar << "\n";
      return false;
  }

  if (!posted) {
    volatile union SimbricksProtoPcieD2H *msg = d2h_alloc(cur_ts);
    volatile struct SimbricksProtoPcieD2HWritecomp &writecomp = msg->writecomp;
    writecomp.req_id = write.req_id;

    SimbricksPcieIfD2HOutSend(&pcieif, msg,
                              SIMBRICKS_PROTO_PCIE_D2H_MSG_WRITECOMP);
  }
  return true;
}

bool h2d_readcomp(volatile struct SimbricksProtoPcieH2DReadcomp &readcomp,
                  uint64_t cur_ts) {
  AXIReader::AXIOperationT &axi_op =
      *(AXIReader::AXIOperationT *)readcomp.req_id;
  memcpy(axi_op.buf, (void *)readcomp.data, axi_op.len);
  axi_op.issued_by->readDone(&axi_op);
  return true;
}

bool h2d_writecomp(volatile struct SimbricksProtoPcieH2DWritecomp &writecomp,
                   uint64_t cur_ts) {
  // noop
  return true;
}

bool poll_h2d(uint64_t cur_ts) {
  volatile union SimbricksProtoPcieH2D *msg =
      SimbricksPcieIfH2DInPoll(&pcieif, cur_ts);

  // no msg available
  if (msg == nullptr)
    return true;

  uint8_t type = SimbricksPcieIfH2DInType(&pcieif, msg);

  switch (type) {
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_READ:
      if (!h2d_read(msg->read, cur_ts)) {
        return false;
      }
      break;
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_WRITE:
      if (!h2d_write(msg->write, cur_ts, false)) {
        return false;
      }
      break;
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_WRITE_POSTED:
      if (!h2d_write(msg->write, cur_ts, true)) {
        return false;
      }
      break;
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_READCOMP:
      if (!h2d_readcomp(msg->readcomp, cur_ts)) {
        return false;
      }
      break;
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_WRITECOMP:
      if (!h2d_writecomp(msg->writecomp, cur_ts)) {
        return false;
      }
      break;
    case SIMBRICKS_PROTO_PCIE_H2D_MSG_DEVCTRL:
    case SIMBRICKS_PROTO_MSG_TYPE_SYNC:
      break; /* noop */
    case SIMBRICKS_PROTO_MSG_TYPE_TERMINATE:
      std::cerr << "poll_h2d: peer terminated\n";
      exiting = true;
      break;
    default:
      std::cerr << "warn: poll_h2d: unsupported type=" << type << "\n";
  }

  SimbricksPcieIfH2DInDone(&pcieif, msg);
  return true;
}

inline void print_cur_ts_if_requested(uint64_t cur_ts) {
  if (print_cur_ts_requested) {
    print_cur_ts_requested = 0;
    std::cout << "current timestamp " << cur_ts << std::endl;
  }
}

int main(int argc, char **argv) {
  if (argc < 3 || argc > 7) {
    std::cerr
        << "Usage: jpeg_decoder_verilator PCI-SOCKET SHM START-TIMESTAMP-PS "
           "SYNC-PERIOD PCI-LATENCY TRACE-FILE\n";
    return EXIT_FAILURE;
  }

  struct SimbricksBaseIfParams if_params;
  memset(&if_params, 0, sizeof(if_params));
  SimbricksPcieIfDefaultParams(&if_params);

  vcontext.time(strtoull(argv[3], nullptr, 0));
  if_params.sync_interval = strtoull(argv[4], nullptr, 0) * 1000ULL;
  if_params.link_latency = strtoull(argv[5], nullptr, 0) * 1000ULL;
  // clock_period = 1000000ULL / strtoull(argv[6], nullptr, 0);

  if_params.sock_path = argv[1];
  if (!PciIfInit(argv[2], if_params)) {
    return EXIT_FAILURE;
  }

  bool sync = SimbricksBaseIfSyncEnabled(&pcieif.base);

  signal(SIGINT, sigint_handler);
  signal(SIGUSR1, sigusr1_handler);

  // setup tracing
  trace_filename = argv[6];
  vcontext.traceEverOn(true);

  // reset chip
  top.pl_resetn0 = 0;  // this signal is active low
  top.pl_clk0 = 0;
  top.eval();
  top.pl_clk0 = 1;
  top.eval();
  top.pl_resetn0 = 1;

  // main simulation loop
  while (!exiting) {
    uint64_t cur_ts = vcontext.time();
    print_cur_ts_if_requested(cur_ts);

    // send required sync messages
    while (SimbricksPcieIfD2HOutSync(&pcieif, cur_ts) < 0) {
      std::cerr << "warn: SimbricksPcieIfD2HOutSync failed cur_ts=" << cur_ts
                << std::endl;
    }

    print_cur_ts_if_requested(cur_ts);

    // process available incoming messages for current timestamp
    do {
      poll_h2d(cur_ts);
    } while (!exiting &&
             ((sync && SimbricksPcieIfH2DInTimestamp(&pcieif) <= cur_ts)));

    // falling edge
    top.pl_clk0 = 0;
    top.eval();
    if (tracing_active) {
      trace.dump(cur_ts - tracing_start);
    }
    vcontext.timeInc(clock_period / 2);

    // input changes to model
    jpeg_decoder_mmio.step(vcontext.time());
    clock_gater_mmio.step(vcontext.time());

    jpeg_decoder_mem_reader.step(vcontext.time());
    jpeg_decoder_mem_writer.step(vcontext.time());

    // evaluate Verilator model for one tick
    top.pl_clk0 = 1;
    top.eval();
    if (tracing_active) {
      trace.dump(vcontext.time() - tracing_start);
    }
    vcontext.timeInc(clock_period / 2);
  }

  trace.close();
  top.final();
  return 0;
}

void JpegDecoderMemReader::doRead(JpegDecoderMemReader::AXIOperationT *op) {
  volatile union SimbricksProtoPcieD2H *msg = d2h_alloc(main_time);
  if (!msg)
    throw "dma read alloc failed";

  unsigned int max_size = SimbricksPcieIfH2DOutMsgLen(&pcieif) -
                          sizeof(SimbricksProtoPcieH2DReadcomp);
  if (op->len > max_size) {
    std::cerr << "error: read data of length " << op->len
              << " doesn't fit into a SimBricks message" << std::endl;
    throw;
  }

  volatile struct SimbricksProtoPcieD2HRead *read = &msg->read;
  read->req_id = (uintptr_t)op;
  read->offset = op->addr;
  read->len = op->len;

  SimbricksPcieIfD2HOutSend(&pcieif, msg, SIMBRICKS_PROTO_PCIE_D2H_MSG_READ);
}

void JpegDecoderMemWriter::doWrite(JpegDecoderMemWriter::AXIOperationT *op) {
  volatile union SimbricksProtoPcieD2H *msg = d2h_alloc(main_time);
  if (!msg)
    throw "dma read alloc failed";

  volatile struct SimbricksProtoPcieD2HWrite *write = &msg->write;
  unsigned int max_size = SimbricksPcieIfH2DOutMsgLen(&pcieif) - sizeof(*write);
  if (op->len > max_size) {
    std::cerr << "error: write data of length " << op->len
              << " doesn't fit into a SimBricks message" << std::endl;
    throw;
  }

  write->req_id = (uintptr_t)op;
  write->offset = op->addr;
  write->len = op->len;
  memcpy((void *)write->data, op->buf, op->len);
  SimbricksPcieIfD2HOutSend(&pcieif, msg, SIMBRICKS_PROTO_PCIE_D2H_MSG_WRITE);

  writeDone(op);
}

void mmio_done(MMIOOp *op, uint64_t cur_ts) {
  if (!op->isWrite) {
    volatile union SimbricksProtoPcieD2H *msg = d2h_alloc(cur_ts);

    if (!msg)
      throw "completion alloc failed";

    volatile struct SimbricksProtoPcieD2HReadcomp *rc = &msg->readcomp;
    memcpy((void *)rc->data, &op->value, op->len);
    rc->req_id = op->id;

    SimbricksPcieIfD2HOutSend(&pcieif, msg,
                              SIMBRICKS_PROTO_PCIE_D2H_MSG_READCOMP);
  }

  delete op;
}
