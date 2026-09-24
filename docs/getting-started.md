# Getting Started

Create a game or application with the quickstart tool, then build it with the commands below. For engine development, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Prerequisites

- [Git](https://www.git-scm.com/)
- [Go](https://go.dev/) (1.26 or later; see the tool go.mod files)
- [CMake](https://cmake.org/) (3.31 or later; 4.2 or later for the [Visual Studio 2026 generator](https://cmake.org/cmake/help/v4.2/generator/Visual%20Studio%2018%202026.html))
- For Windows: Visual Studio 2026 or 2022
- For macOS: a recent Xcode with C++23 support. The current CI selects Xcode 16.4; see [build-macos.yml](../.github/workflows/build-macos.yml).
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
cd tools/cmd/quickstart
go build -o ../../../build/tools/quickstart
cd ../../..
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

This builds Go tools, prepares external binaries, and generates FlatBuffers sources.
Run it before CMake; CMake does not invoke `flatc`. After schema changes, run
`tools/script/build_tools.sh` again, or bootstrap if dependencies/tooling also changed.

## Build assets

```sh
./tools/script/assetbuild.sh
```

This compiles shaders, copies textures and fonts, and packs everything into archive files (`content.idx` + `content.pak`). For details on how the asset pipeline works, see [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md).

## Build your project

### Windows

```sh
# Visual Studio 2026
cmake -S . -B build/windows -G "Visual Studio 18 2026"

# Visual Studio 2022
cmake -S . -B build/windows -G "Visual Studio 17 2022"

cmake --build build/windows --config Debug

./build/windows/Debug/hello_world.exe
```

To edit, build, and debug in Visual Studio, open the generated `.sln` file under
`build/windows/`. The commands above build and run the game without opening the IDE.

### Linux

```sh
cmake -S . -B build/linux -G Ninja \
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
cmake -S . -B build/macos -G Xcode

xcodebuild -project build/macos/hello_world.xcodeproj -configuration Debug

open build/macos/Debug/hello_world.app
```

To edit, build, and debug in Xcode, open `build/macos/hello_world.xcodeproj`.
The commands above build and run the game without opening the IDE.

### Emscripten

```sh
source path/to/emsdk/emsdk_env.sh

cmake -S . -B build/emscripten -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

ninja -C build/emscripten
```

To package the `.js` / `.wasm` output and run the game in a browser, follow the
[Emscripten shipping instructions](shipping.md#emscripten--webassembly).

## Extend the project

To share game logic with a simulator, editor, or test executable, give each
target its own CMakeLists.txt and import Pomdog once in their parent project.
Follow [Using Pomdog with CMake](using-pomdog-with-cmake.md#share-game-logic-between-applications)
for a complete example and the setup required by generated projects.

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
