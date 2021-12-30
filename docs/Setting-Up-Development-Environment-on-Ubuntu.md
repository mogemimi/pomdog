# Setting Up Development Environment on Ubuntu

## Requirements

* Ubuntu 19.04 and later

## Setup environment

#### Updating your system

```sh
sudo apt-get update
sudo apt-get upgrade -y
```

#### Installing latest Clang

```sh
# Installing Clang 13 to Ubuntu 21.04 (hirsute)
sudo apt-get install -y wget gnupg
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/hirsute/ llvm-toolchain-hirsute-13 main" | sudo tee /etc/apt/sources.list.d/llvm.list
echo "deb-src http://apt.llvm.org/hirsute/ llvm-toolchain-hirsute-13 main" | sudo tee -a /etc/apt/sources.list.d/llvm.list
sudo apt-get update
sudo apt-get install -y \
    libllvm13 \
    llvm-13 \
    llvm-13-dev \
    llvm-13-runtime \
    clang-13 \
    clang-tools-13 \
    libclang-13-dev \
    libclang1-13 \
    clang-format-13 \
    clangd-13 \
    libfuzzer-13-dev \
    lldb-13 \
    lld-13 \
    libc++-13-dev \
    libc++abi-13-dev

# To use the latest clang as "clang" and "clang++" commands (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-13 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-13 10

# Installation clean up
sudo apt-get clean
sudo rm -rf /var/lib/apt/lists/*
```

#### Installing Git, Build Tools and Libraries

```sh
sudo apt-get install \
    git \
    ninja-build \
    cmake \
    libc++-dev \
    libc++abi-dev
```

#### Installing OpenGL and OpenAL utilities

```sh
# Installing OpenGL utilities
sudo apt-get install mesa-common-dev libglu1-mesa-dev freeglut3-dev

# Installing OpenAL utilities
sudo apt-get install libopenal1 libopenal-dev
```

## Check your OpenGL version

```sh
sudo apt-get install mesa-utils
glxinfo | grep "OpenGL version"
```

## Docker container and Dockerfile

Please see:

* <https://github.com/mogemimi/gamedev-docker>
* <https://hub.docker.com/r/mogemimi/gamedev-ubuntu/>
* <https://hub.docker.com/r/mogemimi/gamedev-archlinux/>
