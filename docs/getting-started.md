# Getting Started

This guide is for developers who want to make games or applications using Pomdog.
If you want to develop the engine itself, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Prerequisites

- [Git](https://www.git-scm.com/)
- [Go](https://go.dev/)
- [CMake](https://cmake.org/) (3.27 or later)
- For Windows: Visual Studio 2026 or 2022
- For macOS: Xcode 15.2 or newer
- For Linux: Ninja, Clang or GCC. See [Setting Up Development Environment on Ubuntu](setting-up-development-environment-on-ubuntu.md).
- For Emscripten: [Emscripten SDK](https://emscripten.org/). See [Building with Emscripten](building-emscripten.md).

## Clone the Pomdog repository

```sh
git clone https://github.com/mogemimi/pomdog.git
cd pomdog
git submodule update --init --recursive
```

## Build the quickstart tool

```sh
cd pomdog/tools/cmd/quickstart
go build -o ../../../build/tools/quickstart
```

## Create a new project

Use the quickstart tool to generate a new project. The tool copies the `examples/quickstart` template, sets up build scripts, initializes a git repository, and adds Pomdog as a git submodule.

### Interactive mode

```sh
cd path/to/your/directory
./pomdog/build/tools/quickstart -pomdog ./pomdog
```

Example session:

```
$ ./pomdog/build/tools/quickstart -pomdog ./pomdog
> Where is the Pomdog directory? (e.g. path/to/pomdog) [./pomdog]
> Where do you want to create the project? [.]
> Project name? (lowercase letters, digits, underscores; e.g. my_game) hello_world
> Bundle identifier? [com.example.hello-world]
Initializing git repository...
Adding pomdog as git submodule...
Copying template files...
Creating build scripts...
Configuring project files...
Done.

$ cd hello_world
```

### Non-interactive mode (CI / automation)

```sh
./pomdog/build/tools/quickstart \
    -non-interactive \
    -pomdog ./pomdog \
    -name hello_world \
    -o .
```

> **Note:** Project names must start with a lowercase letter and contain only lowercase letters, digits, and underscores (e.g. `my_game`, `hello_world`).

For all available options, see [tools/cmd/quickstart/README.md](../tools/cmd/quickstart/README.md).

## Cloning an existing project

If you clone an existing project that was created with the quickstart tool, initialize the submodules first:

```sh
git clone <your-project-url>
cd your_project
git submodule update --init --recursive
```

## Bootstrap the toolchain

After creating the project, bootstrap the asset pipeline tools:

```sh
cd hello_world
./tools/script/bootstrap.sh
```

This builds the Go-based tools and downloads external binaries (shader compiler, ninja, etc.) into `build/tools/`. This step only needs to be done once.

## Build assets

```sh
./tools/script/assetbuild.sh
```

This compiles shaders, copies textures and fonts, and packs everything into archive files (`content.idx` + `content.pak`). For details on how the asset pipeline works, see [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md).

## Build your project

### Windows

```sh
# Visual Studio 2026
cmake -Bbuild/windows -H. -G "Visual Studio 18"

# Visual Studio 2022
cmake -Bbuild/windows -H. -G "Visual Studio 17"

cmake --build build/windows --config Debug

./build/windows/Debug/hello_world.exe
```

To develop interactively, open the generated `.sln` file in Visual Studio.

### Linux

```sh
cmake -Bbuild/linux -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Debug

ninja -C build/linux

./build/linux/hello_world
```

### macOS

```sh
cmake -Bbuild/macos -H. -G Xcode

xcodebuild -project build/macos/hello_world.xcodeproj -configuration Debug

open build/macos/Debug/hello_world.app
```

To develop interactively, open the generated `.xcodeproj` in Xcode.

### Emscripten

```sh
source path/to/emsdk/emsdk_env.sh

cmake -Bbuild/emscripten -H. -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

ninja -C build/emscripten
```

For more details on build options, compiler flags, and platform-specific configuration, see [Running the Tests](running-the-tests.md).

## Shipping

When your game is ready, create a distributable package:

```sh
# Build assets (if not already done)
./tools/script/assetbuild.sh

# Build Release configuration
cmake --build build/windows --config Release

# Create shipping package
./tools/script/package_windows.sh
```

The packaging scripts are available for Windows, Linux, and Emscripten. For the full shipping workflow on each platform, see [Shipping](shipping.md).
