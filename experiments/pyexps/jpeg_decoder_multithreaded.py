# Copyright 2023 Max Planck Institute for Software Systems, and
# National University of Singapore
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
import os
import glob
import typing as tp
import itertools

import simbricks.orchestration.experiments as exp
import simbricks.orchestration.nodeconfig as node
import simbricks.orchestration.simulators as sim
from simbricks.orchestration.nodeconfig import NodeConfig

class CustomGem5(sim.Gem5Host):

    def __init__(self, node_config: sim.NodeConfig) -> None:
        super().__init__(node_config)
        self.cpu_type = 'O3CPU'
        self.cpu_freq = '3GHz'
        self.mem_sidechannels = []
        self.variant = 'fast'

    def run_cmd(self, env: sim.ExpEnv) -> str:
        cmd = super().run_cmd(env)
        cmd += ' '

        for mem_sidechannel in self.mem_sidechannels:
            cmd += (
                '--simbricks-mem_sidechannel=connect'
                f':{env.dev_mem_path(mem_sidechannel)}'
            )
            cmd += ' '
        return cmd


class JpegDecoderWorkload(node.AppConfig):

    def __init__(
        self,
        dma_addr_start : int,
        num_decoders: int
    ) -> None:
        super().__init__()
        self.dma_addr_start = dma_addr_start
        self.num_decoders = num_decoders

    def prepare_pre_cp(self) -> tp.List[str]:
        return [
            'mount -t proc proc /proc',
            'mount -t sysfs sysfs /sys',
            # enable vfio access to JPEG decoder
            'echo 1 >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode',
            'echo "dead beef" >/sys/bus/pci/drivers/vfio-pci/new_id',
            'sleep 5',
        ]

    def run_cmds(self, node: NodeConfig) -> tp.List[str]:
        cmds = []
        cmds.append(f"/tmp/guest/jpeg_multithreaded_workload 5 {self.num_decoders} {self.dma_addr_start}")
        return cmds

    def config_files(self) -> tp.Dict[str, tp.IO]:
        files = {}
        files["jpeg_multithreaded_workload"] = open("../sims/misc/jpeg_decoder/jpeg_multithreaded_workload", 'rb')

        for img in sorted(glob.glob("../Zurvan/test/data/*.jpg")):
            files[os.path.basename(img)] = open(img, 'rb')

        return files


experiments: tp.List[exp.Experiment] = []
pci_latency = 400
for host_var, jpeg_var, num_decoders in itertools.product(['gem5_kvm', 'gem5_o3'], ['lpn', 'rtl'], [1, 2, 4, 8]):
    e = exp.Experiment(f'jpeg_decoder_multithreaded_post-{host_var}-{jpeg_var}-{num_decoders}')
    node_cfg = node.NodeConfig()
    node_cfg.disk_image = 'base'
    node_cfg.kcmd_append = 'memmap=512M!1G'
    node_cfg.memory = 4 * 1024
    node_cfg.cores = 4
    dma_addr_start = 1024*1024*1024
    node_cfg.app = JpegDecoderWorkload(dma_addr_start, num_decoders)

    if host_var == 'gem5_kvm':
        host = CustomGem5(node_cfg)
        host.cpu_type = 'X86KvmCPU'
        host.sync = False
        node_cfg.nockp = True
    elif host_var == 'gem5_o3':
        e.checkpoint = True
        host = CustomGem5(node_cfg)
        host.sync = True
    else:
        raise NameError(f'Variant {host_var} is unhandled')
    host.wait = True
    host.pci_latency = host.sync_period = pci_latency
    e.add_host(host)

    # add JPEG devices
    for i in range(num_decoders):
        if jpeg_var == 'lpn':
            jpeg_dev = sim.JpegDecoderLpnBmDev()
            if host_var == 'gem5_o3':
                host.mem_sidechannels.append(jpeg_dev)
        elif jpeg_var == 'rtl':
            jpeg_dev = sim.JpegDecoderDev()
        else:
            raise NameError(f'Variant {jpeg_var} is unhandled')
        jpeg_dev.name = f'jpeg{i}'
        jpeg_dev.clock_freq = 2000 # in Mhz
        jpeg_dev.pci_latency = jpeg_dev.sync_period = pci_latency
        host.add_pcidev(jpeg_dev)
        e.add_pcidev(jpeg_dev)

    experiments.append(e)
