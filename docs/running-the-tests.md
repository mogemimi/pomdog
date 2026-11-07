# Running the Tests

For prerequisites and setup, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Linux

```sh
cd path/to/pomdog

# Generate Ninja files (Clang + libc++)
cmake -S . -B build/linux -G "Ninja Multi-Config" \
    -DCMAKE_MAKE_PROGRAM="$PWD/build/tools/ninja" \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi"

# Build
cmake --build build/linux --config Debug

# Run tests
./build/linux/tests/Debug/pomdog_tests
```

For custom toolchains (GCC, Clang with libstdc++), static linking of libc++, and shared library bundling, see [Building for Linux](building-linux.md).

## macOS

```sh
cd path/to/pomdog

# Generate Xcode project
cmake -S . -B build/macos -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

# Build
xcodebuild -project build/macos/pomdog.xcodeproj -configuration Debug

# Run tests
./build/macos/tests/Debug/pomdog_tests
```

## Windows

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -S . -B build/windows -G "Visual Studio 18 2026"

# Visual Studio 2022
cmake -S . -B build/windows -G "Visual Studio 17 2022"

# Build
cmake --build build/windows --config Debug

# Run tests
./build/windows/tests/Debug/pomdog_tests.exe
```

## Tests that read files

Some tests read files from the Pomdog checkout. They run only when the command
line names the checkout, and are skipped otherwise:

```sh
./build/linux/tests/Debug/pomdog_tests --pomdog-dir=path/to/pomdog
```

The Visual Studio debugger and the generated Xcode scheme pass the checkout of
the CMake configuration. The web tests run without it, because Node.js runs them
in a virtual file system.

## Network tests

The network tests build into `pomdog_network_tests`. They talk to local servers,
so run them with
[network-test-runner](../tools/cmd/network-test-runner/README.md), which starts
the servers and passes their addresses:

```sh
./build/tools/network-test-runner ./build/linux/tests/Debug/pomdog_network_tests
```

Options after the executable go to doctest. The web build holds only the HTTP
client and WebSocket tests; see
[Building with Emscripten](building-emscripten.md) to run it.

## Emscripten

See [Building with Emscripten](building-emscripten.md).

For independent CMake library consumers, see [Engine Library Link Tests](engine-library-link-tests.md).
