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
# Installing Clang 11 to Ubuntu 20.04 (focal)
sudo apt-get install -y wget gnupg
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | tee /etc/apt/sources.list.d/llvm.list
echo "deb-src http://apt.llvm.org/focal/ llvm-toolchain-focal-11 main" | tee -a /etc/apt/sources.list.d/llvm.list
sudo apt-get update
sudo apt-get install -y \
    libllvm11 \
    llvm-11 \
    llvm-11-dev \
    llvm-11-runtime \
    clang-11 \
    clang-tools-11 \
    libclang-11-dev \
    libclang1-11 \
    clang-format-11 \
    clangd-11 \
    libfuzzer-11-dev \
    lldb-11 \
    lld-11 \
    libc++-11-dev \
    libc++abi-11-dev

# To use the latest clang as "clang" and "clang++" commands (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-11 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-11 10

# Installation clean up
sudo apt-get clean
sudo rm -rf /var/lib/apt/lists/*
```

#### Installing Git, Build Tools and Libraries

```sh
sudo apt-get install \
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
