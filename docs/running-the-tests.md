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

To compile with Clang and libstdc++:

```sh
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libstdc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libstdc++" \
    -DCMAKE_BUILD_TYPE=Debug
```

### Statically linking libc++

When distributing binaries built with Clang and libc++ to other Linux machines,
you may encounter `libc++.so.1` or `libc++abi.so.1` not found errors.
To avoid this, statically link libc++ and libc++abi into the executable:

```sh
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -Wl,-Bstatic -lc++ -lc++abi -Wl,-Bdynamic -lpthread -ldl" \
    -DCMAKE_BUILD_TYPE=Release
```

### Bundling shared libraries with RPATH

Alternatively, you can bundle `libc++.so.1` and `libc++abi.so.1` alongside the
executable and set RPATH so the loader finds them at runtime:

```sh
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_INSTALL_RPATH="\$ORIGIN/lib" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_BUILD_TYPE=Release
```

Then copy the shared libraries next to the built binary:

```sh
mkdir -p build/linux_release/tests/lib
cp /usr/lib/x86_64-linux-gnu/libc++.so.1    build/linux_release/tests/lib/
cp /usr/lib/x86_64-linux-gnu/libc++abi.so.1 build/linux_release/tests/lib/
```

The resulting directory layout:

```
build/linux_release/
  tests/pomdog_tests
  lib/
    libc++.so.1
    libc++abi.so.1
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
