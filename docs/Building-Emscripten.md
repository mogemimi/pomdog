# Building Emscripten

- Docker image: [mogemimi/gamedev-emscripten](https://hub.docker.com/r/mogemimi/gamedev-emscripten/)

## Download and Install emsdk

```sh
# Get the emsdk repository
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

- https://emscripten.org/docs/getting_started/downloads.html

## Build

```sh
# Enter the emsdk directory
cd path/to/emsdk

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh

# Enter your pomdog repository
cd path/to/pomdog

# Generate Ninja file
cmake -Bbuild/emscripten -H. \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

# Build
ninja -C build/emscripten

# Run test
node \
    --experimental-modules \
    --experimental-wasm-modules \
    --experimental-wasm-threads \
    ./build/emscripten/test/PomdogTest.js
```
