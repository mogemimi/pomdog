# Developing Pomdog Game Engine

To build the engine, set up the tools below and bootstrap the checkout. For creating a game, see [Getting Started](getting-started.md).

## Prerequisites

The following software is required:

- [Git](https://www.git-scm.com/)
- [CMake](https://cmake.org/) (3.31 or later; 4.2 or later for the [Visual Studio 2026 generator](https://cmake.org/cmake/help/v4.2/generator/Visual%20Studio%2018%202026.html))
- [Go](https://go.dev/) >= 1.26
- For Windows:
  - Visual Studio 2026 (recommended) or Visual Studio 2022
- For macOS:
  - A recent Xcode with C++23 support. The current [macOS CI](../.github/workflows/build-macos.yml) selects Xcode 16.4.
- For Linux:
  - [Ninja](https://ninja-build.org/)
  - Clang >= 21.0 or GCC >= 15.2
  - See [Setting Up Development Environment on Ubuntu](setting-up-development-environment-on-ubuntu.md)

For library selection and application integration, see [Using Pomdog with CMake](using-pomdog-with-cmake.md).
For the build definition layout and compiler policies, see [CMake Build Settings](cmake-build-settings.md).

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

The `bootstrap.sh` script runs `tools/cmd/bootstrap-toolchain`, a Go-based bootstrapper.

Bootstrap also generates FlatBuffers C++ and Go sources in the engine checkout's
`build/schemas-cpp` and `build/schemas-go`. Run it before configuring CMake.

- Run bootstrap after cloning and when dependencies or tooling change.
- After schema changes, run `tools/script/build_tools.sh` again; a full bootstrap also regenerates the sources.
- For a clean bootstrap, remove the generated `build` directory and run bootstrap again. This also removes compiled applications and generated assets.

See [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md) for asset generation.

## Building on Windows

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -S . -B build/windows -G "Visual Studio 18 2026"

# Visual Studio 2022
cmake -S . -B build/windows -G "Visual Studio 17 2022"

# Build
cmake --build build/windows --config Debug
cmake --build build/windows --config Release

# Run tests
./build/windows/tests/Debug/pomdog_tests.exe
./build/windows/tests/Release/pomdog_tests.exe
```

Use Visual Studio 2026 for engine development. CI also covers Visual Studio 2022.

### clang-cl

To build with clang-cl, install the Visual Studio components "C++ Clang Compiler
for Windows" and "MSBuild support for LLVM (clang-cl) toolset", then select the
ClangCL toolset with `-T ClangCL`:

```sh
cd path/to/pomdog

# Visual Studio 2026 with clang-cl
cmake -S . -B build/windows_clangcl -G "Visual Studio 18 2026" -T ClangCL

# Build
cmake --build build/windows_clangcl --config Debug
cmake --build build/windows_clangcl --config Release

# Run tests
./build/windows_clangcl/tests/Debug/pomdog_tests.exe
./build/windows_clangcl/tests/Release/pomdog_tests.exe
```

The ClangCL toolset links with `lld-link`, which Release LTO builds require.
AddressSanitizer is not configured for clang-cl; use MSVC for ASan builds.

## Building on macOS

```sh
cd path/to/pomdog

# Generate Xcode project
cmake -S . -B build/macos -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

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

# Generate Ninja files
cmake -S . -B build/linux -G "Ninja Multi-Config" \
    -DCMAKE_MAKE_PROGRAM="$PWD/build/tools/ninja" \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi"

# Build
cmake --build build/linux --config Debug
cmake --build build/linux --config Release

# Run tests
./build/linux/tests/Debug/pomdog_tests
./build/linux/tests/Release/pomdog_tests
```

For GCC users:

```sh
# Generate Ninja files with GCC
cmake -S . -B build/linux -G "Ninja Multi-Config" \
    -DCMAKE_MAKE_PROGRAM="$PWD/build/tools/ninja" \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++
```

## Building with Emscripten

See [Building with Emscripten](building-emscripten.md).

## Address Sanitizer builds

Address Sanitizer (ASan) helps detect memory corruption bugs such as out-of-bounds access and use-after-free.
Enable it by passing `-DPOMDOG_USE_ADDRESS_SANITIZER=1` to CMake.
ASan defaults LTO to OFF in a fresh cache. When switching an existing build, also
set `-DPOMDOG_USE_LTO=OFF`; requesting both is an error. Bundled dependencies are
instrumented as well. MemorySanitizer is not a supported build configuration.

### Windows (MSVC)

```sh
cd path/to/pomdog

# Visual Studio 2026
cmake -S . -B build/windows_asan -G "Visual Studio 18 2026" -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Visual Studio 2022
cmake -S . -B build/windows_asan -G "Visual Studio 17 2022" -DPOMDOG_USE_ADDRESS_SANITIZER=1

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
cmake -S . -B build/macos_asan -G Xcode -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Build
xcodebuild -project build/macos_asan/pomdog.xcodeproj -configuration Debug
xcodebuild -project build/macos_asan/pomdog.xcodeproj -configuration Release

# Run tests
./build/macos_asan/tests/Debug/pomdog_tests
./build/macos_asan/tests/Release/pomdog_tests
```

### Linux (Clang)

```sh
cd path/to/pomdog

# Generate Ninja files with ASan
cmake -S . -B build/linux_asan -G "Ninja Multi-Config" \
    -DCMAKE_MAKE_PROGRAM="$PWD/build/tools/ninja" \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DPOMDOG_USE_ADDRESS_SANITIZER=1

# Build
cmake --build build/linux_asan --config Debug
cmake --build build/linux_asan --config Release

# Run tests
./build/linux_asan/tests/Debug/pomdog_tests
./build/linux_asan/tests/Release/pomdog_tests
```

For GCC, use the same ASan option with `-DCMAKE_C_COMPILER=gcc` and
`-DCMAKE_CXX_COMPILER=g++`.

## Code formatting

Pomdog uses two code formatting tools:

1. **clang-format**: Formats C++ source files.
2. **plain-text-format** (`tools/cmd/plain-text-format`): A Go tool that performs basic text cleanup: removes trailing whitespace, converts CRLF to LF, and ensures files end with a newline.

Both tools are applied to C++ source files. Markdown, CMake, and other text files are formatted with `plain-text-format` only.

For large codebases, `tools/cmd/clang-format-all` wraps clang-format to run in parallel for faster execution.

Run the formatting scripts from the checkout root:

```sh
cd path/to/pomdog

# Format C++ source files (clang-format + plain-text-format)
./tools/script/clang_format.sh

# Format documentation and CMake files (plain-text-format only)
./tools/script/docs_format.sh
```

## CI build matrix

The checked-in [GitHub Actions workflows](../.github/workflows) define the following build configurations:

| Platform | Toolchain | Configurations |
|:---|:---|:---|
| Windows | MSVC (Visual Studio 2026),<br>MSVC (Visual Studio 2022) | Debug, Release,<br>ASan + Debug, ASan + Release |
| Windows | clang-cl (Visual Studio 2026) | Debug, Release |
| macOS | Apple Clang (Xcode) | Debug, Release,<br>ASan + Debug, ASan + Release |
| Linux (Arch Linux) | GCC w/ libstdc++ | Debug, Release,<br>ASan + Debug, ASan + Release |
| Linux (Ubuntu) | Clang w/ libc++ | Debug, Release,<br>ASan + Debug, ASan + Release |
| Linux (Ubuntu) | Clang w/ libstdc++ | Debug, Release |
| Emscripten | Emscripten (emcc/em++) | Debug, Release |

The [Linux workflow](../.github/workflows/build-linux.yml) also runs
[engine library link tests](engine-library-link-tests.md) on Ubuntu with Clang/libc++, in Debug
and Release with LTO disabled.
