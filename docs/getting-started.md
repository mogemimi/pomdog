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

## Clone the Pomdog repository

```sh
git clone https://github.com/mogemimi/pomdog.git
cd pomdog
git submodule update --init --recursive
```

## Bootstrap toolchain

Run the bootstrap script to set up the asset pipeline tools:

```sh
./tools/script/bootstrap.sh
```

This only needs to be done once after cloning. For more details, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md#bootstrap-toolchain).

## Create a new project

Use the quickstart tool to generate a new project:

```sh
cd pomdog/tools/cmd/quickstart
go build

cd path/to/your/directory
./pomdog/tools/cmd/quickstart/quickstart
```

Example session:

```
$ ./pomdog/tools/cmd/quickstart/quickstart
> Where is a Pomdog directory? (e.g. path/to/pomdog) ./pomdog
> Where do you want to create your new gamedev project? [.] .
> What is your project name? (e.g. MyGame) HelloWorld
> What is your project URL? [com.example.HelloWorld]
Done.

$ cd HelloWorld
```

## Build your project

### Linux

```sh
cd path/to/HelloWorld

cmake -Bbuild -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Debug

ninja -C build

./build/HelloWorld
```

### macOS

```sh
cd path/to/HelloWorld

cmake -Bbuild -H. -G Xcode
xcodebuild -project build/HelloWorld.xcodeproj -configuration Debug

open build/Debug/HelloWorld.app
```

### Windows

```sh
cd path/to/HelloWorld

# Visual Studio 2026
cmake -Bbuild -H. -G "Visual Studio 18"

# Visual Studio 2022
cmake -Bbuild -H. -G "Visual Studio 17"

cmake --build build --config Debug

./build/Debug/HelloWorld.exe
```

To develop interactively, open the generated `.xcodeproj` or `.sln` file in Xcode or Visual Studio.
