# Running the Tests

## Build and run the unit tests on Linux

To build with CMake and Ninja, run the following commands:

```sh
cd path/to/pomdog

# Generate Ninja file to the 'build.cmake' directory
cmake -Bbuild.cmake -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Release

# Compiling source code
ninja -C build.cmake

# Run the unit tests
./build.cmake/test/PomdogTest
```

To run the 'FeatureShowcase' application, use the following:

```sh
./build.cmake/examples/FeatureShowcase/FeatureShowcase
```

### Building with a custom toolchain

To compile with GCC, run the following command:

```sh
cmake -Bbuild.cmake -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release
```

You can also override the compiler options by setting the environment variables such as `expor CXX=...` or `export CXXFLAGS=...`.

```sh
export CC=clang
export CXX=clang++
export LINK=clang++
export CXXFLAGS="-std=c++17 -stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
cmake -Bbuild.cmake -H. -G Ninja -DCMAKE_BUILD_TYPE=Release
```

## Build and run the unit tests on Mac OS X

To run all unit tests, use:

```sh
cd path/to/pomdog

# Generate Xcode project files to the 'build.cmake' directory
cmake -Bbuild.cmake -H. -G Xcode

# Compiling source code
xcodebuild -project Pomdog.xcodeproj -configuration Release

# Run the unit tests
./build.cmake/test/Release/PomdogTest
```

To run the 'FeatureShowcase' application, use the following:

```sh
open ./build.cmake/examples/FeatureShowcase/Release/FeatureShowcase.app
```

## Build and run the unit tests on Windows

```sh
cd path/to/pomdog

# Generate projects for Visual Studio 2019 to the 'build.cmake' directory
cmake -Bbuild.cmake -H. -G "Visual Studio 16"

# Building projects using CMake and MSBuild
cmake --build build.cmake --config Release

# To run all unit tests, use the following:
./build.cmake/test/Release/PomdogTest
```

```sh
cd path/to/pomdog

# Move to the `examples/FeatureShowcase` directory
cd examples/FeatureShowcase

# To run the 'FeatureShowcase' application, use the following:
./../../build.cmake/examples/FeatureShowcase/Release/FeatureShowcase.exe
```
