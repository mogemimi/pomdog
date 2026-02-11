# Running the Tests

For prerequisites and setup, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Linux

```sh
cd path/to/pomdog

# Generate Ninja files (Clang + libc++)
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Debug

# Build
ninja -C build/linux_debug

# Run tests
./build/linux_debug/tests/pomdog_tests
```

### Building with a custom toolchain

To compile with GCC instead:

```sh
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Debug
```

You can also set the compiler via environment variables:

```sh
export CC=clang
export CXX=clang++
export CXXFLAGS="-stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
cmake -Bbuild/linux_debug -H. -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

## macOS

```sh
cd path/to/pomdog

# Generate Xcode project
cmake -Bbuild/macos -H. -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

# Build
xcodebuild -project build/macos/pomdog.xcodeproj -configuration Debug

# Run tests
./build/macos/tests/Debug/pomdog_tests
```

## Windows

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -Bbuild/windows -H. -G "Visual Studio 18"

# Visual Studio 2022
cmake -Bbuild/windows -H. -G "Visual Studio 17"

# Build
cmake --build build/windows --config Debug

# Run tests
./build/windows/tests/Debug/pomdog_tests.exe
```

## Emscripten

See [Building with Emscripten](building-emscripten.md).
