# Setting Up Development Environment on Ubuntu

## Requirements

- Ubuntu 24.04 or later

## Setup environment

### Update your system

```sh
sudo apt-get update
sudo apt-get upgrade -y
```

### Install Clang 18

```sh
sudo apt-get install -y wget gnupg

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-18 main" | sudo tee /etc/apt/sources.list.d/llvm.list
echo "deb-src http://apt.llvm.org/noble/ llvm-toolchain-noble-18 main" | sudo tee -a /etc/apt/sources.list.d/llvm.list

sudo apt-get update
sudo apt-get install -y \
    libllvm18 \
    llvm-18 \
    llvm-18-dev \
    llvm-18-runtime \
    clang-18 \
    clang-tools-18 \
    libclang-18-dev \
    libclang1-18 \
    clang-format-18 \
    clangd-18 \
    libfuzzer-18-dev \
    lldb-18 \
    lld-18 \
    libc++-18-dev \
    libc++abi-18-dev

# Set clang-18 as the default clang/clang++ (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 10

# Clean up
sudo apt-get clean
sudo rm -rf /var/lib/apt/lists/*
```

### Install build tools and libraries

```sh
sudo apt-get install -y \
    build-essential \
    git \
    ninja-build \
    cmake \
    golang-go \
    libc++-dev \
    libc++abi-dev
```

### Install OpenGL and OpenAL

```sh
# OpenGL
sudo apt-get install -y mesa-common-dev libglu1-mesa-dev freeglut3-dev

# OpenAL
sudo apt-get install -y libopenal1 libopenal-dev
```

## Check your OpenGL version

```sh
sudo apt-get install -y mesa-utils
glxinfo | grep "OpenGL version"
```

## Docker

Pre-built Docker images are available for CI and development:

- [mogemimi/gamedev-ubuntu](https://hub.docker.com/r/mogemimi/gamedev-ubuntu/) (Ubuntu)
- [mogemimi/gamedev-archlinux](https://hub.docker.com/r/mogemimi/gamedev-archlinux/) (Arch Linux)
- Source: <https://github.com/mogemimi/gamedev-docker>
