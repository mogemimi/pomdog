# Setting Up Development Environment on Ubuntu

## Requirements

* Ubuntu 19.04 and later

## Setup environment

#### Updating your system

```sh
sudo apt update
sudo apt upgrade -y
```

#### Installing latest Clang

```sh
# Installing Clang 8
sudo apt install -y wget gnupg
echo "deb http://apt.llvm.org/disco/ llvm-toolchain-disco-8 main" | tee /etc/apt/sources.list.d/llvm.list
echo "deb-src http://apt.llvm.org/disco/ llvm-toolchain-disco-8 main" | tee -a /etc/apt/sources.list.d/llvm.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt update
sudo apt install -y \
    clang-8 \
    libclang-common-8-dev \
    libclang-8-dev \
    libclang1-8 \
    libllvm8 \
    llvm-8 \
    llvm-8-dev \
    llvm-8-runtime \
    lldb-8 \
    lld-8 \
    clang-format-8

# To use the latest clang as "clang" and "clang++" commands (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-8 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-8 10

# Installation clean up
sudo rm -rf /var/lib/apt/lists/*
sudo apt clean
```

#### Installing Git, Build Tools and Libraries

```sh
sudo apt install \
    git \
    make \
    ninja-build \
    cmake \
    libc++-dev \
    libc++abi-dev
```

#### Installing OpenGL and OpenAL utilities

```sh
# Installing OpenGL utilities
sudo apt install mesa-common-dev libglu1-mesa-dev freeglut3-dev

# Installing OpenAL utilities
sudo apt install libopenal1 libopenal-dev
```

## Check your OpenGL version

```sh
sudo apt install mesa-utils
glxinfo | grep "OpenGL version"
```

## Docker container and Dockerfile

Please see:

* <https://github.com/mogemimi/gamedev-docker>
* <https://hub.docker.com/r/mogemimi/gamedev-ubuntu/>
* <https://hub.docker.com/r/mogemimi/gamedev-archlinux/>
