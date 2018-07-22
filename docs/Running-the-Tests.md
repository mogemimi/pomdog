# Running the Tests

## Build and run the unit tests on Linux

```sh
cd path/to/pomdog

# Creating a build directory
mkdir -p build.cmake && cd build.cmake

# Generate Makefile
export CC=clang
export CXX=clang++
export LINK=clang++
export CXXFLAGS="-std=c++17 -stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
cmake -DCMAKE_BUILD_TYPE=Release ../build/PomdogTest/

# Compiling source code
make

# Run the unit tests
./PomdogTest        
```

## Build and run the unit tests on Mac OS X

To run all unit tests, use:

```sh
cd path/to/pomdog

# Creating a build directory
mkdir -p build.cmake && cd build.cmake

# Generating Xcode project files
cmake -G Xcode ../build/PomdogTest/

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
cmake -G "Visual Studio 15" ../build/PomdogTest/

# Building projects using CMake and MSBuild
cmake --build . --config Release

# To run all unit tests, use the following:
./Release/PomdogTest
```
