# Building Emscripten

- Docker image: [mogemimi/gamedev-emscripten](https://hub.docker.com/r/mogemimi/gamedev-emscripten/)

```sh
cd path/to/emsdk
source ./emsdk_env.sh

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
