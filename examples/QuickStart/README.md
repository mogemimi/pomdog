# QuickStart

## Prerequisites

#### Runtime requirements

* Mac OS X 10.11 and later
* Windows 10 and later
* Ubuntu 18.04 LTS and later
* DirectX 11, OpenGL4 or Metal

#### Build requirements

* CMake 3.10 and later
* Clang 6.0 (for Linux)
* Xcode 9.2 and later
* Visual Studio 2017 and later

## How to build

### Building under Linux

```sh
cd path/to/QuickStart

# Creating a build directory
mkdir -p build && cd build

# Generating Makefile
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug ..

# Building application
make -j4

# To run your application, you can use the following
./QuickStart
```

To build in release mode, use `-DCMAKE_BUILD_TYPE` option:

```sh
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Building under Mac and Xcode

```sh
cd path/to/QuickStart

# Creating a build directory
mkdir -p build && cd build

# Generating Xcode project files
cmake -G Xcode ..

# Building your project
xcodebuild -project QuickStart.xcodeproj -configuration Debug

# To run your application, you can use the following
open Debug/QuickStart.app
```

To build in release mode, use `-configuration` option:

```sh
xcodebuild -project QuickStart.xcodeproj -configuration Release
```

To develop your application on Xcode, please open `QuickStart.xcodeproj` in Xcode.

### Building under Visual Studio 2017

```sh
# Git Bash (MinGW)
cd path/to/QuickStart

# Creating a build directory
mkdir -p build && cd build

# Generating projects for Visual Studio 2017
cmake -G "Visual Studio 15" ..

# Building projects using CMake and MSBuild
cmake --build . --config Debug

# To run your application, you can use the following
./Debug/QuickStart
```

To build in release mode, use `--config` option:

```sh
cmake --build . --config Release
```

To develop your application on Visual Studio, please open `QuickStart.sln` in Visual Studio.
