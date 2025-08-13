FROM ubuntu:24.04

RUN DEBIAN_FRONTEND=noninteractive apt update && apt install -y --no-install-recommends \
    apt-transport-https \
    curl \
    gnupg \
    autoconf \
    bc \
	bison \
    patch \
	cmake \
    make \
    g++-10 \
	lld \
	flex \
	git \
	libboost-coroutine-dev \
	libboost-fiber-dev \
	libboost-iostreams-dev \
	libelf-dev \
	libglib2.0-dev \
	libgoogle-perftools-dev \
	libpcap-dev \
	libpixman-1-dev \
	ninja-build \
    pkg-config \
    mold \
    libcapstone-dev \
    openjdk-17-jdk \
	python-is-python3 \
	python3-dev \
    python3-pip \
    python3-setuptools \
    python3-opencv \ 
    python3-numpy \
    python3-decorator \
    python3-typing-extensions \
    python3-scipy \
    python3-psutil \
    python3-attr \
    python3-packaging \
    python3-pytest \
    python3-cloudpickle \
    python3-cffi \
    python3-matplotlib \
    python3-tornado \
	rsync \
	scons \
	unzip \
    bzip2 \
    xz-utils \
	wget \
	vim \
    tmux \
    perl \
    verilator \
    less \
    util-linux \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    clang-18 \
    libzstd-dev \
    libzstd-dev \
    meson \
    systemd \
    llvm-17-dev \
    libbpf-dev \
    sudo \
    ssh \
    jq \
    locales \
    direnv

RUN pip --no-cache-dir install --break-system-packages mxnet

# Set g++-10 and gcc-10 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-10 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100 \
    && update-alternatives --set gcc /usr/bin/gcc-10 \
    && update-alternatives --set cc /usr/bin/gcc-10 \
    && update-alternatives --set g++ /usr/bin/g++-10

# Locale (UTF-8)
RUN sed -i 's/# \(en_US.UTF-8 UTF-8\)/\1/' /etc/locale.gen \
    && locale-gen
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

# Remove sudo password
ARG USERNAME=ubuntu
RUN mkdir -p /etc/sudoers.d \
    && echo "${USERNAME} ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/90-${USERNAME} \
    && chmod 0440 /etc/sudoers.d/90-${USERNAME}

# Create a default workspace folder (VS Code will bind mount the project here)
RUN mkdir -p /SimBricks-LPN && chown ${USERNAME}:${USERNAME} /SimBricks-LPN
WORKDIR /SimBricks-LPN

USER ${USERNAME}

# install sbt
RUN echo "deb https://repo.scala-sbt.org/scalasbt/debian all main" | sudo tee /etc/apt/sources.list.d/sbt.list \
    && echo "deb https://repo.scala-sbt.org/scalasbt/debian /" | sudo tee /etc/apt/sources.list.d/sbt_old.list \
    && curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" | sudo -H gpg --no-default-keyring --keyring gnupg-ring:/etc/apt/trusted.gpg.d/scalasbt-release.gpg --import \
    && sudo chmod 644 /etc/apt/trusted.gpg.d/scalasbt-release.gpg \
    && sudo apt update \
    && sudo DEBIAN_FRONTEND=noninteractive apt install sbt

# reduce image size
RUN sudo rm -rf /var/lib/apt/lists/*

RUN mkdir -p /home/${USERNAME}/.config/direnv \
    && echo 'eval "$(direnv hook bash)"' >> /home/${USERNAME}/.bashrc \ 
    &&  echo '[whitelist]' > /home/ubuntu/.config/direnv/direnv.toml \
    && echo 'prefix = ["/SimBricks-LPN"]' >> /home/ubuntu/.config/direnv/direnv.toml
