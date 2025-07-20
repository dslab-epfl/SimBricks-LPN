import math

import simbricks.orchestration.experiments as exp
import simbricks.orchestration.simulators as sim
import simbricks.orchestration.nodeconfig as node
import itertools

experiments = []

# Experiment parameters
host_variants = ["gk", "go3"]
vta_ops = ["rtl", "lpn"]
inference_device_opts = [
    node.TvmDeviceType.VTA,
    node.TvmDeviceType.CPU,
    node.TvmDeviceType.CPU_AVX512,
]
model_name_opts = [
    "resnet18_v1",
    "resnet34_v1",
    "resnet50_v1",
]
num_vta_opts = [1, 2, 4, 8]


class TvmClassifyLocal(node.AppConfig):
    """Runs inference for detection model locally, either on VTA or the CPU."""

    def __init__(self):
        super().__init__()
        self.pci_vta_id_start = 0
        self.num_vta_devices = 1
        self.device = node.TvmDeviceType.VTA
        self.repetitions = 1
        self.batch_size = 1
        self.vta_batch = 1
        self.vta_block = 16
        self.model_name = "resnet18_v1"
        self.debug = True
        self.gem5_cp = False

    def config_files(self):
        # mount TVM inference script in simulated server under /tmp/guest
        import os
        home = os.environ.get("HOME")
        return {
            "pci_driver.cc":
                open(
                    f"{home}/SimBricks-LPN/local/pci_driver.cc",
                    "rb",
                ),
            "test.c":
                open(
                    f"{home}/SimBricks-LPN/local/test.c",
                    "rb",
                ),
            "cat.png":
                open(f"{home}/SimBricks-LPN/local/data/cat.png", "rb"),
            "multi_classification-infer.py":
                open(
                    f"{home}/SimBricks-LPN/local/tvm/vta/tutorials/frontend/multi_classification-infer.py",
                    "rb"
                )
        }

    def prepare_pre_cp(self):
        cmds = super().prepare_pre_cp()
        cmds.extend([
            'echo \'{"TARGET" : "simbricks-pci", "HW_VER" : "0.0.2",'
            ' "LOG_INP_WIDTH" : 3, "LOG_WGT_WIDTH" : 3,'
            ' "LOG_ACC_WIDTH" : 5, "LOG_BATCH" :'
            f' {int(math.log2(self.vta_batch))}, "LOG_BLOCK" :'
            f' {int(math.log2(self.vta_block))}, "LOG_UOP_BUFF_SIZE" :'
            ' 15, "LOG_INP_BUFF_SIZE" : 15, "LOG_WGT_BUFF_SIZE" : 18,'
            ' "LOG_ACC_BUFF_SIZE" : 17 }\' >'
            " /root/tvm/3rdparty/vta-hw/config/vta_config.json"
        ])
        return cmds

    def run_cmds(self, node):
        # define commands to run on simulated server
        # cmds = [
        # "cp /tmp/guest/pci_driver.cc /root/tvm/3rdparty/vta-hw/src/simbricks-pci/pci_driver.cc",
        # "cd /root/tvm/build",
        # "make clean",
        # "make -j4",
        # ]
        cmds = []
        cmds.extend([
            # start RPC server
            f"export TVM_NUM_THREADS=1 ",
            f"python3 -m tvm.exec.rpc_tracker --host=0.0.0.0 --port=9091 &",
            "sleep 10"
        ])
        for i in range(self.num_vta_devices):
            cmds.append(
                f"VTA_DEVICE=0000:00:{(self.pci_vta_id_start + i):02x}.0 VTA_VFIO_GROUP_ID={i} python3 -m vta.exec.rpc_server --key=simbricks-pci --tracker=127.0.0:9091 &"
            )
            cmds.append(
               "sleep 2"
            )
        cmds.extend([
            # wait for RPC servers to start
            "sleep 10",
        ])
        cmds.extend([
            # wait for RPC servers to start
            "python3 -m tvm.exec.query_rpc_tracker --host 0.0.0.0 --port 9091",
        ])
        if self.gem5_cp:
            cmds.append("export GEM5_CP=1")
        # cmds.extend([
        #     "dmesg | grep -i mtrr",
        #     "dmesg | grep -i pat"])
        # run inference
        cmds.append((
            "python3 /tmp/guest/multi_classification-infer.py /root/mxnet"
            f" {self.device.value} {self.model_name} /tmp/guest/cat.png"
            f" {self.batch_size} {self.repetitions} {int(self.debug)} 0 {self.num_vta_devices}"
        ))

        return cmds


class VtaNode(node.NodeConfig):

    def __init__(self) -> None:
        super().__init__()
        # Use locally built disk image
        self.disk_image = "vta_classification"
        # Bump amount of system memory
        self.memory = 4 * 1024
        # Reserve physical range of memory for the VTA user-space driver
        # 1G is the start 1G - 1G+512M
        self.kcmd_append = "memmap=1G$1G iomem=relaxed"
        # self.kcmd_append = "memmap=512M@1G iomem=relaxed"
        # self.kcmd_append = "iomem=relaxed"

    def prepare_pre_cp(self):
        # Define commands to run before application to configure the server
        cmds = super().prepare_pre_cp()
        cmds.extend([
            "mount -t proc proc /proc",
            "mount -t sysfs sysfs /sys",
            "mkdir /dev/shm",
            "mount -t tmpfs tmpfs /dev/shm",
            # Make TVM's Python framework available
            "export PYTHONPATH=/root/tvm/python:${PYTHONPATH}",
            "export PYTHONPATH=/root/tvm/vta/python:${PYTHONPATH}",
            "export MXNET_HOME=/root/mxnet",
            # Set up loopback interface so the TVM inference script can
            # connect to the RPC server
            "ip link set lo up",
            "ip addr add 127.0.0.1/8 dev lo",
            # Make VTA device available for control from user-space via
            # VFIO
            (
                "echo 1"
                " >/sys/module/vfio/parameters/enable_unsafe_noiommu_mode"
            ),
            'echo "dead beef" >/sys/bus/pci/drivers/vfio-pci/new_id',
        ])

        return cmds


# Build experiment for all combinations of parameters
for (
    host_var,
    inference_device,
    model_name,
    vta_op,
    num_vta
) in itertools.product(
    host_variants,
    inference_device_opts,
    model_name_opts,
    vta_ops,
    num_vta_opts
):
    experiment = exp.Experiment(
        f"classify_multi_exit-{model_name}-{inference_device.value}-{host_var}-{vta_op}-{num_vta}"
    )
    
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

    # Instantiate server
    HostClass = CustomGem5
    server_cfg = VtaNode()
    server_cfg.nockp = True
    server_cfg.cores = 8
    server_cfg.app = TvmClassifyLocal()
    server_cfg.app.device = inference_device
    server_cfg.app.model_name = model_name
    server_cfg.app.pci_vta_id_start = 5
    server_cfg.app.gem5_cp = True
    server_cfg.app.num_vta_devices = num_vta
    server = HostClass(server_cfg)
    # Whether to synchronize VTA and server
    if host_var == "gk":
        server.cpu_type = 'X86KvmCPU'
        server.sync = False
    elif host_var == "go3":
        server.sync = True
        experiment.checkpoint = True
    # Wait until server exits
    server.wait = True

    pci_latency = 400

    # Instantiate and connect VTA PCIe-based accelerator to server
    if inference_device == node.TvmDeviceType.VTA:
        for i in range(num_vta):
            if vta_op == "rtl":
                vta = sim.VTADev()
            else:
                vta = sim.VTALpnBmDev()
                server.mem_sidechannels.append(vta)

            vta.clock_freq = 2000
            vta.name = f"vta{i}"
            vta.pci_latency = vta.sync_period = pci_latency
            server.add_pcidev(vta)

    server.pci_latency = server.sync_period = pci_latency

    # Add both simulators to experiment
    experiment.add_host(server)
    experiment.add_pcidev(vta)

    experiments.append(experiment)
