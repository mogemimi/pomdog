# Setting Up Development Environment on Ubuntu

## Requirements

* Ubuntu 18.04 and later

## Setup environment

#### Updating your system

```sh
sudo apt update
sudo apt upgrade -y
```

#### Installing latest Clang

```sh
# Installing Clang 6.0
sudo apt install -y wget gnupg
echo "deb http://apt.llvm.org/artful/ llvm-toolchain-artful-6.0 main" | tee /etc/apt/sources.list.d/llvm.list
echo "deb-src http://apt.llvm.org/artful/ llvm-toolchain-artful-6.0 main" | tee -a /etc/apt/sources.list.d/llvm.list
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
sudo apt update
sudo apt install -y \
    clang-6.0 \
    libclang-common-6.0-dev \
    libclang-6.0-dev \
    libclang1-6.0 \
    libllvm6.0 \
    llvm-6.0 \
    llvm-6.0-dev \
    llvm-6.0-runtime \
    lldb-6.0 \
    python-lldb-6.0 \
    clang-format-6.0

# To use the latest clang as "clang" and "clang++" commands (optional)
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-6.0 10
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-6.0 10

# Installation clean up
sudo rm -rf /var/lib/apt/lists/*
sudo apt clean
```

#### Installing Git, Build Tools and Libraries

```sh
sudo apt install
    git \
    make \
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
