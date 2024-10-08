# Getting Started

## Prerequisites

The following software is required in order to create and build a new project:

- [Git](https://www.git-scm.com/)
- [Golang](https://golang.org/)
- [CMake](https://cmake.org/)
- For Linux:
  - [Ninja](https://ninja-build.org/)
  - Clang or GCC
  - For more details about installation of build requirements, please see [Setting Up Development Environment on Ubuntu](Setting-Up-Development-Environment-on-Ubuntu.md).
- For Windows:
  - Visual Studio
- For macOS:
  - Xcode

## Clone the repo with Git

1. Clone the repository

    ```sh
    git clone https://github.com/mogemimi/pomdog.git
    ```

2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init --recursive
    ```

## Create a new project

```sh
go get -u github.com/fatih/color
go get -u github.com/pkg/errors
go run pomdog/tools/quickstart/main.go
```

e.g.

```sh
$ git clone https://github.com/mogemimi/pomdog.git && cd pomdog
$ git submodule update --init --recursive
$ cd ..
$ go run pomdog/tools/quickstart/main.go
> Where is a Pomdog directory? (e.g. path/to/pomdog) ./pomdog
> Where do you want to create your new gamedev project? [.] .
> What is your project name? (e.g. MyGame) HelloWorld
> What is your project URL? [com.example.HelloWorld]
Done.

$ cd HelloWorld
```

## Build with CMake on Linux

* [Setting Up Development Environment on Ubuntu](Setting-Up-Development-Environment-on-Ubuntu.md)

```sh
cd path/to/HelloWorld

# Create a build directory
mkdir -p build && cd build

# Generate Makefile using CMake
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug ..

# Build application
make -j4

# To run your application, you can use the following
./HelloWorld
```

To choose build mode, use the `-DCMAKE_BUILD_TYPE=Debug` or `-DCMAKE_BUILD_TYPE=Release` option:

```sh
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release ..
```

## Build with Xcode on Mac OS X

```sh
cd path/to/HelloWorld

# Create a build directory
mkdir -p build && cd build

# Generate the Xcode project file
cmake -G Xcode ..

# Build your project
xcodebuild -project HelloWorld.xcodeproj -configuration Debug

# To run your application, you can use the following
open Debug/HelloWorld.app
```

To build in release mode, use `-configuration` option:

```sh
xcodebuild -project HelloWorld.xcodeproj -configuration Release
```

To develop your application on Xcode, please open `HelloWorld.xcodeproj` in Xcode.

## Build with Visual Studio 2022 on Windows

```sh
# Git Bash (MinGW)
cd path/to/HelloWorld

# Create your 'build' directory and generate projects for Visual Studio 2022
cmake -Bbuild -H. -G "Visual Studio 17"

# Building projects using CMake and MSBuild
cmake --build build --config Debug

# To run your application, you can use the following
./build/Debug/HelloWorld
```

To build in release mode, use `--config` option:

```sh
cmake --build build --config Release
```

To develop your application on Visual Studio, please open `HelloWorld.sln` in Visual Studio.
