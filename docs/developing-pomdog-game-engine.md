# Developing Pomdog Game Engine

This guide is for developers who want to build and contribute to the Pomdog engine itself.
If you want to make games using Pomdog, see [Getting Started](getting-started.md) instead.

## Prerequisites

The following software is required:

- [Git](https://www.git-scm.com/)
- [CMake](https://cmake.org/) (3.27 or later)
- [Go](https://go.dev/) >= 1.25
- For Windows:
  - Visual Studio 2026 (recommended) or Visual Studio 2022
- For macOS:
  - Xcode 15.2 or newer
- For Linux:
  - [Ninja](https://ninja-build.org/)
  - Clang >= 21.0 or GCC >= 15.2
  - See [Setting Up Development Environment on Ubuntu](setting-up-development-environment-on-ubuntu.md)

## Clone the repository

```sh
git clone https://github.com/mogemimi/pomdog.git
cd pomdog
git submodule update --init --recursive
```

## Bootstrap toolchain

Before building, run the bootstrap script to set up the asset pipeline tools.
The bootstrap downloads and builds the required tools (written in Go and third-party OSS) into the `build/tools` directory.

```sh
./tools/script/bootstrap.sh
```

The `bootstrap.sh` script internally runs `tools/cmd/bootstrap-toolchain`, a Go-based bootstrapper.

- You only need to run this once after cloning the repository.
- Re-run it when you update dependencies or tooling.
- To perform a clean bootstrap, delete the `build` directory and run again.

## Building on Windows

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -Bbuild/windows -H. -G "Visual Studio 18"

# Visual Studio 2022
cmake -Bbuild/windows -H. -G "Visual Studio 17"

# Build
cmake --build build/windows --config Debug
cmake --build build/windows --config Release

# Run tests
./build/windows/tests/Debug/pomdog_tests.exe
./build/windows/tests/Release/pomdog_tests.exe
```

Visual Studio 2026 is the primary development environment going forward.
Visual Studio 2022 is still supported for CI but will eventually be removed.

## Building on macOS

```sh
cd path/to/pomdog

# Generate Xcode project
cmake -Bbuild/macos -H. -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

# Build
xcodebuild -project build/macos/pomdog.xcodeproj -configuration Debug
xcodebuild -project build/macos/pomdog.xcodeproj -configuration Release

# Run tests
./build/macos/tests/Debug/pomdog_tests
./build/macos/tests/Release/pomdog_tests
```

## Building on Linux

See [Setting Up Development Environment on Ubuntu](setting-up-development-environment-on-ubuntu.md) for installing dependencies.

```sh
cd path/to/pomdog

# Generate Ninja files (Debug)
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Debug

# Generate Ninja files (Release)
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_BUILD_TYPE=Release

# Build
ninja -C build/linux_debug
ninja -C build/linux_release

# Run tests
./build/linux_debug/tests/pomdog_tests
./build/linux_release/tests/pomdog_tests
```

For GCC users:

```sh
# Generate Ninja files with GCC (Debug)
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Debug

# Generate Ninja files with GCC (Release)
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release
```

## Building with Emscripten

See [Building with Emscripten](building-emscripten.md).

## Address Sanitizer builds

Address Sanitizer (ASan) helps detect memory corruption bugs such as out-of-bounds access and use-after-free.
Enable it by passing `-DPOMDOG_USE_ADDRESS_SANITIZER=1` to CMake.

### Windows (MSVC)

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -Bbuild/windows_asan -H. -G "Visual Studio 18" -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Visual Studio 2022
cmake -Bbuild/windows_asan -H. -G "Visual Studio 17" -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Build
cmake --build build/windows_asan --config Debug
cmake --build build/windows_asan --config Release

# Run tests
./build/windows_asan/tests/Debug/pomdog_tests.exe
./build/windows_asan/tests/Release/pomdog_tests.exe
```

On Windows, ASan-enabled executables require the following runtime DLLs:

- `clang_rt.asan_dynamic-x86_64.dll`
- `clang_rt.asan_dbg_dynamic-x86_64.dll`
- `llvm-symbolizer.exe`

Use the `pomdog_copy_asan_runtime` CMake function to automatically copy these files to the build output directory:

```cmake
include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
pomdog_copy_asan_runtime(pomdog_tests)
```

### macOS (Xcode)

```sh
cd path/to/pomdog

# Generate Xcode project with ASan
cmake -Bbuild/macos_asan -H. -G Xcode -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Build
xcodebuild -project build/macos_asan/pomdog.xcodeproj -configuration Debug
xcodebuild -project build/macos_asan/pomdog.xcodeproj -configuration Release

# Run tests
./build/macos_asan/tests/Debug/pomdog_tests
./build/macos_asan/tests/Release/pomdog_tests
```

### Linux (clang)

```sh
cd path/to/pomdog

# Generate Ninja files with ASan (Debug)
cmake -Bbuild/linux_asan_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DPOMDOG_USE_ADDRESS_SANITIZER=1 \
    -DCMAKE_BUILD_TYPE=Debug

# Generate Ninja files with ASan (Release)
cmake -Bbuild/linux_asan_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DPOMDOG_USE_ADDRESS_SANITIZER=1 \
    -DCMAKE_BUILD_TYPE=Release

# Build
ninja -C build/linux_asan_debug
ninja -C build/linux_asan_release

# Run tests
./build/linux_asan_debug/tests/pomdog_tests
./build/linux_asan_release/tests/pomdog_tests
```

## Code formatting

Pomdog uses two code formatting tools:

1. **clang-format** — Formats C++ source files.
2. **plain-text-format** (`tools/cmd/plain-text-format`) — A Go tool that performs basic text cleanup: removes trailing whitespace, converts CRLF to LF, and ensures files end with a newline.

Both tools are applied to C++ source files. Markdown, CMake, and other text files are formatted with `plain-text-format` only.

For large codebases, `tools/cmd/clang-format-all` wraps clang-format to run in parallel for faster execution.

Convenience scripts are provided:

```sh
cd path/to/pomdog

# Format C++ source files (clang-format + plain-text-format)
./tools/script/clang_format.sh

# Format documentation and CMake files (plain-text-format only)
./tools/script/docs_format.sh
```

## CI build matrix

GitHub Actions runs the following build configurations:

| Platform | Configurations |
|:---|:---|
| Windows | Debug, Release, ASan + Debug, ASan + Release |
| macOS | Debug, Release, ASan + Debug, ASan + Release |
| Linux (Arch Linux) | GCC + libc++ (Debug, Release) |
| Linux (Ubuntu) | Clang + libc++ (Debug, Release), Clang + libstdc++ (Debug, Release) |
| Emscripten | Debug, Release |
