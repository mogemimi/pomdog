# Running the Tests

## Requirements

Please see this page [Developing Pomdog Game Engine](Developing-Pomdog-Game-Engine.md).

## Build and run the unit tests on Linux

To build with CMake and Ninja, run the following commands:

```sh
cd path/to/pomdog

# Generate Ninja file to the 'build/linux' directory
cmake -Bbuild/linux -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Release

# Compiling source code
ninja -C build/linux

# Run the unit tests
./build/linux/test/PomdogTest
```

To run the 'FeatureShowcase' application, use the following:

```sh
./build/linux/examples/FeatureShowcase/FeatureShowcase
```

After a complete build, you will find the `libpomdog.a` file in `build/linux/cmake/pomdog` directory.

### Building with a custom toolchain

To compile with GCC, run the following command:

```sh
cmake -Bbuild/linux -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release
```

You can also override the compiler options by setting the environment variables such as `expor CXX=...` or `export CXXFLAGS=...`.

```sh
export CC=clang
export CXX=clang++
export LINK=clang++
export CXXFLAGS="-std=c++20 -stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
cmake -Bbuild/linux -H. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

## Build and run the unit tests on macOS

First, you need to generate the Xcode project file (.xcodeproj) using CMake. Afterwards you can build the whole project.

```sh
cd path/to/pomdog

# Generate Xcode project files to the 'build/macos' directory
cmake -Bbuild/macos -H. -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

# Compiling source code
xcodebuild -project build/macos/Pomdog.xcodeproj -configuration Release
```

To build in debug mode, use `-configuration` option:

```sh
xcodebuild -project Pomdog.xcodeproj -configuration Debug
```

After a complete build, you will find the `libpomdog.a` file in `build/macos/cmake/pomdog/Release` (also `build/macos/cmake/pomdog/Debug`) directory.

To run all unit tests, use:

```sh
# Run the unit tests
./build/macos/test/Release/PomdogTest
```

To run the 'FeatureShowcase' application, use the following:

```sh
open ./build/macos/examples/FeatureShowcase/Release/FeatureShowcase.app
```

## Build and run the unit tests on Windows

First, you need to generate the Visual Studio project files (.sln and .vcxproj) using CMake. Afterwards you can build the whole project.

```sh
cd path/to/pomdog

# Generate projects for Visual Studio 2022 to the 'build/windows' directory
cmake -Bbuild/windows -H. -G "Visual Studio 17"

# Building projects using CMake and MSBuild
cmake --build build/windows --config Release
```

To switch between Debug/Release modes, use the `--config` option:

```sh
cmake --build build/windows --config Debug
```

After a complete build, you will find the `libpomdog.lib` file in `build/windows/build/Release` (also `build/windows/build/Debug`) directory.

To run all unit tests, use:

```sh
# To run all unit tests, use the following:
./build/windows/test/Release/PomdogTest
```

To run the 'FeatureShowcase' application, use the following:

```sh
cd path/to/pomdog

# Move to the `examples/FeatureShowcase` directory
cd examples/FeatureShowcase

# To run the 'FeatureShowcase' application, use the following:
./../../build/windows/examples/FeatureShowcase/Release/FeatureShowcase.exe
```
