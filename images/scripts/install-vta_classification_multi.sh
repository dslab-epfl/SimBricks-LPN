#!/bin/bash
set -eux

cp /tmp/tvm-my-copy/3rdparty/vta-hw/src/simbricks-pci/pci_driver.cc.100MB /root/tvm/3rdparty/vta-hw/src/simbricks-pci/pci_driver.cc
cd /root/tvm
cd build
cmake ..
make -j`nproc`

export PYTHONPATH=/root/tvm/python:/root/tvm/vta/python
export VTA_CFG=/root/tvm/3rdparty/vta-hw/config/vta_config.json