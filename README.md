# LPN/DSim for SimBricks

This repo contains LPN/Dsim integration for SimBricks.

The repo assumes this project is cloned under $HOME/, if not, some path may not work. 
 
# Build Procedure

Clone the repo
```
git clone https://github.com/dslab-epfl/SimBricks-LPN.git

git submodule update --init --recursive
```

Follow these steps from the repository root:

1. **Compile the whole project:**
   ```bash
   make
   ```
2. **Build gem5/QEMU/Simulators:**
   ```bash
    # run `apt remove libprotobuf-dev protobuf-compiler libprotoc-dev` if the following command has issue with protobuf 

    CC=gcc-10 CXX=g++-10 make sims/external/gem5/ready
 
    CC=gcc-10 CXX=g++-10 make sims/external/qemu/ready
   ```
4. **Build disk images:**
   ```bash
   make build-images
   ```
5. **Convert the built disk image to raw format:**
   ```bash
   make convert-images-raw
   ```
   
6. **Compile RTLs**

   In the following, we will make RTL simulators, 

   ```bash
   sudo apt install openjdk-17-jdk
   ```

   In case you have java installed already, use the following one to choose the right version of java
   ```bash
   sudo update-alternatives --config java 
   sudo update-alternatives --config javac
   ```

   Install sbt

   ```bash
   sudo apt-get update
   sudo apt-get install apt-transport-https curl gnupg -yqq
   echo "deb https://repo.scala-sbt.org/scalasbt/debian all main" | sudo tee /etc/apt/sources.list.d/sbt.list
   echo "deb https://repo.scala-sbt.org/scalasbt/debian /" | sudo tee /etc/apt/sources.list.d/sbt_old.list
   curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" | sudo -H gpg --no-default-keyring --keyring gnupg-ring:/etc/apt/trusted.gpg.d/scalasbt-release.gpg --import
   sudo chmod 644 /etc/apt/trusted.gpg.d/scalasbt-release.gpg
   sudo apt-get update
   sudo apt-get install sbt
   ```
   
   Finally, make the RTL simulators
   ```bash

    unset VTA_HW_PATH

    # check if sims/external/vta is on lpn branch
    # need to install sbt
    CC=gcc-10 CXX=g++-10 make sims/external/vta/ready

    CC=gcc-10 CXX=g++-10 make sims/external/protoacc/ready
   ```

7. **Run the experiment:**
The following scripts launch tmux session and run individual experiments there
```bash
cd experiments
./run_jpeg.sh
./run_vta.sh
./run_protoacc.sh
```
  

## DSim models in SimBricks
- `lib/simbricks/pciebm`: 
- `sims/lpn`: DSim/LPN simulator for accelerators (JPEG Decoder, Protoacc, VTA)
