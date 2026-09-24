# Engine Library Link Tests

Pomdog engine developers run these tests after changing CMake library dependencies.
Game developers do not need them to build games with Pomdog.

`pomdog_tests` and the examples link several libraries at once. An explicit link
can supply a dependency that one library forgot to declare. In
`tests/cmake/engine_library_link_tests`, each consumer links one `pomdog::` alias without
`pomdog_app_settings`. Existing unit tests call compiled symbols so the linker
checks that library's declared dependencies. The consumers also check its public
include paths and C++ requirements.

## Run locally

Initialize submodules and run the
[bootstrap procedure](developing-pomdog-game-engine.md#bootstrap-toolchain) before
configuring. Some libraries need the generated schema headers. Run these commands
from the repository root and give this CMake project its own build directory.

### Linux

Use Clang, libc++, and Ninja:

```sh
cmake -S tests/cmake/engine_library_link_tests -B build/engine_library_link_tests_linux \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DPOMDOG_USE_LTO=OFF \
    -DPOMDOG_EXCLUDE_FROM_ALL=ON
cmake --build build/engine_library_link_tests_linux
ctest --test-dir build/engine_library_link_tests_linux --output-on-failure --no-tests=error
```

### Windows

Run from Git Bash with Visual Studio 2026:

```sh
cmake -S tests/cmake/engine_library_link_tests -B build/engine_library_link_tests_windows \
    -G "Visual Studio 18 2026" \
    -DPOMDOG_USE_LTO=OFF \
    -DPOMDOG_EXCLUDE_FROM_ALL=ON
cmake --build build/engine_library_link_tests_windows --config Debug
ctest --test-dir build/engine_library_link_tests_windows -C Debug --output-on-failure
```

### Emscripten

Activate emsdk and use Ninja:

```sh
emcmake cmake -S tests/cmake/engine_library_link_tests -B build/engine_library_link_tests_emscripten \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPOMDOG_USE_LTO=OFF \
    -DPOMDOG_EXCLUDE_FROM_ALL=ON
cmake --build build/engine_library_link_tests_emscripten
ctest --test-dir build/engine_library_link_tests_emscripten --output-on-failure
```

CTest uses the Node emulator from the Emscripten toolchain. The Emscripten build
omits `experimental_async_link_test`.

These commands use Debug. To run Release with Ninja, configure another build
directory with `-DCMAKE_BUILD_TYPE=Release`. With Visual Studio, build with
`--config Release` and run CTest with `-C Release`.

Use `POMDOG_USE_LTO=OFF` to expose unresolved symbols during linking. Set
`POMDOG_EXCLUDE_FROM_ALL=ON` so CMake builds each consumer and its declared
dependencies.

## Check CPU-only dependencies

Use a fresh build directory and build only the CPU consumers:

```sh
cmake -S tests/cmake/engine_library_link_tests -B build/engine_library_link_tests_cpu \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPOMDOG_USE_LTO=OFF \
    -DPOMDOG_EXCLUDE_FROM_ALL=ON
cmake --build build/engine_library_link_tests_cpu --config Debug \
    --target math_link_test random_link_test vfs_link_test application_link_test
ctest --test-dir build/engine_library_link_tests_cpu -C Debug \
    -R "^(math|random|vfs|application)_link$" --output-on-failure --no-tests=error
```

Check the link commands and build output for GPU, audio, network, or platform
host implementations. None should compile or link. CMake still defines unused
library targets with `POMDOG_EXCLUDE_FROM_ALL=ON`. You can build `math_link_test`
without generated schema headers.

These consumers use selected symbols; passing tests cannot establish that every
public API links. Use separate tests for GPU device creation, host loops, and
archive loading.

## CI

The [Linux workflow](../.github/workflows/build-linux.yml) runs these tests on
Ubuntu with Clang/libc++ in Debug and Release, with LTO disabled.
