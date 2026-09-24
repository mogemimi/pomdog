# Setting Up Development Environment on Ubuntu

## Requirements

- Ubuntu 26.04 (Resolute), the release targeted by these commands
- A C++23-capable compiler
- CMake 3.31 or later
- Go 1.26 or later, as required by the checked-in Go tool modules

## Install Clang and build tools

Ubuntu 26.04 provides [clang-21](https://packages.ubuntu.com/en/resolute/devel/clang-21)
and the matching libc++ / libc++abi development packages in its repositories.
Enable the Universe repository if it is not already available. There is no need
to add the Ubuntu 24.04 (Noble) LLVM repository to a 26.04 installation.

```sh
sudo apt-get update
sudo apt-get install -y \
    build-essential git ninja-build cmake \
    clang-21 clang-tools-21 clang-format-21 clangd-21 lld-21 \
    libc++-21-dev libc++abi-21-dev
```

Check `cmake --version` (3.31 or later) and `go version` (1.26 or later).
Install Go using the [official installation instructions](https://go.dev/doc/install)
if the distribution package is older than the Go tool modules require.

## Install X11, OpenGL, and OpenAL

Install the system dependencies for the engine libraries you build. The current Linux
graphical host uses X11, OpenGL / GLX, and OpenAL. CPU-only applications using
libraries such as math, random, or VFS do not require these libraries; see
[Using Pomdog with CMake](using-pomdog-with-cmake.md) for selecting libraries and
excluding unused ones from the build.

### X11

The `application_gpu` library uses X11 for windows and platform events:

```sh
sudo apt-get install -y libx11-dev
```

### OpenGL

The OpenGL backend in `gpu` and the host's GLX context setup need OpenGL
development headers and libraries. Pomdog builds its bundled GLEW sources:

```sh
sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
```

### OpenAL

The `audio` library uses OpenAL on Linux. The graphical host also links this library:

```sh
sudo apt-get install -y libopenal-dev
```

### Check OpenGL

Check OpenGL in a running desktop / X11 session:

```sh
sudo apt-get install -y mesa-utils
glxinfo -B
```

## Configure Pomdog

Clone the repository and complete the
[bootstrap procedure](developing-pomdog-game-engine.md#bootstrap-toolchain) first.
Then run the following from the Pomdog checkout.

Use explicit compiler names when configuring a fresh build directory:

```sh
cmake -S . -B build/linux_debug -G Ninja \
    -DCMAKE_C_COMPILER=clang-21 \
    -DCMAKE_CXX_COMPILER=clang++-21 \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Debug
```

For other Ubuntu releases, use packages for that release or follow the matching distribution
instructions at [LLVM's package repository](https://apt.llvm.org/).

Continue with [Developing Pomdog](developing-pomdog-game-engine.md) for bootstrap,
asset generation, build, and tests. Package availability alone does not verify
the graphical backend on a particular GPU / driver.

## Docker

Pre-built images are used by the [Linux CI workflow](../.github/workflows/build-linux.yml):

- [mogemimi/gamedev-ubuntu](https://hub.docker.com/r/mogemimi/gamedev-ubuntu/)
- [mogemimi/gamedev-archlinux](https://hub.docker.com/r/mogemimi/gamedev-archlinux/)
- Source: [mogemimi/gamedev-docker](https://github.com/mogemimi/gamedev-docker)
