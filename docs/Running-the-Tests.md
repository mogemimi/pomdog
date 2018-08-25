# Running the Tests

## Build and run the unit tests on Linux

```sh
cd path/to/pomdog

# Creating a build directory
mkdir -p build.cmake && cd build.cmake

# Generate Makefile
cmake \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Release \
    ../test

# Compiling source code
make -j4

# Run the unit tests
./PomdogTest        
```

### Building with a custom toolchain

To compile with GCC, run the following command:

```sh
cmake \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release \
    ../test
```

You can also override the compiler options by setting the environment variables such as `expor CXX=...` or `export CXXFLAGS=...`.

```sh
export CC=clang
export CXX=clang++
export LINK=clang++
export CXXFLAGS="-std=c++17 -stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
cmake -DCMAKE_BUILD_TYPE=Release ../test
```

### Building with Ninja instead of GNU Make

To build with Ninja, run the following command:

```sh
# Generate Ninja file
cmake \
    -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Release \
    ../test

# Build with Ninja
ninja
```

## Build and run the unit tests on Mac OS X

To run all unit tests, use:

```sh
cd path/to/pomdog

# Creating a build directory
mkdir -p build.cmake && cd build.cmake

# Generating Xcode project files
cmake -G Xcode ../test

# Compiling source code
xcodebuild -project PomdogTest.xcodeproj -configuration Release

# Run the unit tests
./Release/PomdogTest
```

## Build and run the unit tests on Windows

```sh
cd path/to/pomdog

# Creating a build directory
mkdir build.cmake
cd build.cmake

# Generating projects for Visual Studio 2017
cmake -G "Visual Studio 15" ../test

# Building projects using CMake and MSBuild
cmake --build . --config Release

# To run all unit tests, use the following:
./Release/PomdogTest
```
