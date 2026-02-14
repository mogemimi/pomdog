# Setting Up Development Environment on Ubuntu

## Requirements

- Ubuntu 24.04 or later

## Setup environment

### Install Clang 21

```sh
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    ca-certificates \
    gnupg \
    wget
sudo rm -rf /var/lib/apt/lists/*

mkdir -p /etc/apt/keyrings
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key \
    | gpg --dearmor -o /etc/apt/keyrings/llvm.gpg
echo "deb [signed-by=/etc/apt/keyrings/llvm.gpg] http://apt.llvm.org/noble/ llvm-toolchain-noble-21 main" \
    > /etc/apt/sources.list.d/llvm.list

sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    libllvm21 \
    llvm-21 \
    llvm-21-dev \
    llvm-21-runtime \
    clang-21 \
    clang-tools-21 \
    libclang-21-dev \
    libclang1-21 \
    clang-format-21 \
    clangd-21 \
    libfuzzer-21-dev \
    lldb-21 \
    lld-21 \
    libc++-21-dev \
    libc++abi-21-dev

# Set clang-21 as the default clang/clang++ (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-21 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 10
sudo update-alternatives --install /usr/bin/ld ld /usr/bin/ld.lld-21 10

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

### Install X11, OpenGL, and OpenAL

```sh
# X11
sudo apt-get install -y libx11-dev

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
