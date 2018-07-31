# Getting Started

## Clone the repo with Git

1. Clone the repository

    ```sh
    git clone https://github.com/mogemimi/pomdog.git
    ```

2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init
    ```

## Create a new project

```sh
python pomdog/tools/quickstart.py
```

e.g.

```sh
$ git clone https://github.com/mogemimi/pomdog.git
$ python pomdog/tools/quickstart.py
Configure your new project

> Where do you want to create your new gamedev project? [.] .
> What is your project name? (e.g. MyGame) HelloWorld
> What is your project URL? [com.example.HelloWorld]
HelloWorld: Create a new directory.
Create a new project at 'HelloWorld'.
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

## Build with Visual Studio 2017 on Windows

```sh
# Git Bash (MinGW)
cd path/to/HelloWorld

# Creating a build directory
mkdir -p build && cd build

# Generating projects for Visual Studio 2017
cmake -G "Visual Studio 15" ..

# Building projects using CMake and MSBuild
cmake --build . --config Debug

# To run your application, you can use the following
./Debug/HelloWorld
```

To build in release mode, use `--config` option:

```sh
cmake --build . --config Release
```

To develop your application on Visual Studio, please open `HelloWorld.sln` in Visual Studio.
