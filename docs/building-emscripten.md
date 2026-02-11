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

## Docker

A Docker image is also available for Emscripten builds:

- [mogemimi/gamedev-emscripten](https://hub.docker.com/r/mogemimi/gamedev-emscripten/)
