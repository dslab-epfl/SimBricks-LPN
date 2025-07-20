
# LPN/DSim for SimBricks
This repo contains LPN/Dsim integration for SimBricks.
 

## Repository Structure
- `doc/`: Documentation (Sphinx), automatically deployed on
  [Read The Docs](https://simbricks.readthedocs.io/en/latest/?badge=latest).
- `lib/simbricks/`: Libraries implementing SimBricks interfaces
  - `lib/simbricks/base`: Base protocol implementation responsible for
    connection setup, message transfer, and time synchronization between
    SimBricks component simulators.
  - `lib/simbricks/network`: Network protocol implementation carrying Ethernet
    packets between network components. Layers over the base protocol.
  - `lib/simbricks/pcie`: PCIe protocol implementation, roughly modelling PCIe
    at the transaction level, interconnecting hosts with PCIe device simulators.
    Layers over base protocol.
  - `lib/simbricks/nicbm`: Helper C++ library for implementing behavioral
    (high-level) NIC simulation models, offers similar abstractions as device
    models in other simulators such as gem-5.
  - `lib/simbricks/nicif`: *(deprecated)* Thin C library for NIC simulators
    establishing a network and a PCIe connection.
- `dist/`: Proxies for distributed SimBricks simulations running on multiple
  physical hosts.
  - `dist/sockets/`: Proxy transporting SimBricks messages over regular TCP
    sockets.
  - `dist/rdma/`: RDMA SimBricks proxy (not compiled by default).
- `sims/`: Component Simulators integrated into SimBricks.
  - `sims/external/`: Submodule pointers to repositories for existing external
    simulators (gem5, QEMU, Simics, ns-3, FEMU).
  - `sims/nic/`: NIC simulators
    - `sims/nic/i40e_bm`: Behavioral NIC model for Intel X710 40G NIC.
    - `sims/nic/corundum`: RTL simulation with Verilator of the
      [Corundum FPGA NIC](https://corundum.io/).
    - `sims/nic/corundum_bm`: Simple behavioral Corundum NIC model.
    - `sims/nic/e1000_gem5`: E1000 NIC model extracted from gem5.
  - `sims/net/`: Network simulators
    - `sims/net/net_switch`: Simple behavioral Ethernet switch model.
    - `sims/net/wire`: Simple Ethernet "wire" connecting two NICs back-to-back.
    - `sims/net/pktgen`: Packet generator.
    - `sims/net/tap`: Linux TAP device adapter.
    - `sims/net/tofino/`: Adapter for Intel Tofino Simulator.
    - `sims/net/menshen`: RTL simulation with Verilator for the
      [Menshen RMT Pipeline](https://isolation.quest/).
  - `sims/lpn`: DSim/LPN simulator for accelerators (JPEG Decoder, Protoacc, VTA)
- `experiments/`: Python orchestration framework for running simulations.
  - `experiments/simbricks/orchestration/`: Orchestration framework implementation.
  - `experiments/run.py`: Main script for running simulation experiments.
  - `experiments/pyexps/`: Example simulation experiments.
- `images/`: Infrastructure to build disk images for host simulators.
  - `images/kernel/`: Slimmed down Linux kernel to reduce simulation time.
  - `images/mqnic/`: Linux driver for Corundum NIC.
  - `images/scripts/`: Scripts for installing packages in disk images.
- `docker/`: Scripts for building SimBricks Docker images.
