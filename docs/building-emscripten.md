# Building with Emscripten

This guide covers building Pomdog for WebAssembly using Emscripten.
Assumes you have already cloned the repository and run the [bootstrap](developing-pomdog-game-engine.md#bootstrap-toolchain).

## Install emsdk

```sh
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

See also: <https://emscripten.org/docs/getting_started/downloads.html>

## Build

```sh
# Activate emsdk
cd path/to/emsdk
source ./emsdk_env.sh

cd path/to/pomdog

# (Optional) Add Ninja to PATH if not installed system-wide
PATH=$PATH:./build/tools

# Generate Ninja files (Debug)
cmake -Bbuild/emscripten_debug -H. -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

# Generate Ninja files (Release)
cmake -Bbuild/emscripten_release -H. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

# Build
ninja -C build/emscripten_debug
ninja -C build/emscripten_release
```

## Run tests

The emsdk provides Node.js, so no separate installation is needed.

```sh
node \
    --experimental-modules \
    --experimental-wasm-modules \
    ./build/emscripten_debug/tests/pomdog_tests.js
```

## Run examples in a browser

After building assets and the Release configuration, create a shipping package and launch a local web server:

```sh
# Build assets (if not already done)
./tools/script/bootstrap.sh
./tools/script/assetbuild.sh

# Package the Emscripten build output
./tools/script/package_emscripten.sh

# Open in browser
emrun --browser chrome ./build/feature_showcase/shipping/web/index.html
```

For more details on packaging and deployment, see [Shipping](shipping.md).

## Threading (pthread)

By default, Pomdog runs in single-threaded mode on Emscripten. To enable pthread support, pass `-DPOMDOG_ENABLE_EMSCRIPTEN_PTHREAD=1` to CMake:

```sh
cmake -Bbuild/emscripten_debug -H. -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
    -DPOMDOG_ENABLE_EMSCRIPTEN_PTHREAD=1
```

When pthread is enabled, Emscripten compiles with `-pthread` and links with `-s USE_PTHREADS=1`. This requires the browser to support `SharedArrayBuffer`, which the web server must explicitly enable by sending these HTTP headers:

```
Cross-Origin-Opener-Policy: same-origin
Cross-Origin-Embedder-Policy: require-corp
```

Without these headers, the browser will disable `SharedArrayBuffer` and the application will fail to start.

## Memory

Games typically require large amounts of memory. Pomdog's example applications default to `-s ALLOW_MEMORY_GROWTH`, which allows the WebAssembly linear memory to grow dynamically at runtime. Custom applications should also set this flag if needed (in CMake: `"SHELL:-s ALLOW_MEMORY_GROWTH"`).

## Docker

A Docker image is also available for Emscripten builds:

- [mogemimi/gamedev-emscripten](https://hub.docker.com/r/mogemimi/gamedev-emscripten/)
