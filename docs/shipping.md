# Shipping

This document describes how to create a distributable package for a Pomdog application. The general workflow is:

1. Build assets into archive files
2. Build the runtime executable (Release configuration)
3. Combine the archive files and executable into a shipping package

For details on the asset build pipeline, see [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md).

## Windows

```sh
# 1. Bootstrap the pipeline tools
./tools/script/bootstrap.sh

# 2. Build assets
./tools/script/assetbuild.sh

# 3. Build the runtime (Release)
cmake -Bbuild/windows -H. -G "Visual Studio 18"
cmake --build build/windows --config Release

# 4. Create the shipping package
./tools/script/package_windows.sh
```

The packaging script copies the Release `.exe` and the desktop archive files (`content.idx`, `content.pak`) into `build/<app>/shipping/windows/`:

```
build/<app>/shipping/windows/
├── <app>.exe
├── content.idx
└── content.pak
```

This directory can be distributed as-is (e.g. compressed into a `.zip`).

## Linux

```sh
# 1. Bootstrap the pipeline tools
./tools/script/bootstrap.sh

# 2. Build assets
./tools/script/assetbuild.sh

# 3. Build the runtime (Release, with RPATH configured)
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_INSTALL_RPATH="\$ORIGIN/lib" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_BUILD_TYPE=Release
ninja -C build/linux_release

# 4. Create the shipping package
./tools/script/package_linux.sh
```

The resulting package:

```
build/<app>/shipping/linux/
├── <app>
├── content.idx
├── content.pak
└── lib/
    ├── libc++.so.1
    ├── libc++abi.so.1
    └── ...
```

### Shared Library Bundling

When building with Clang and `-stdlib=libc++` (the default), the executable depends on `libc++.so.1` and `libc++abi.so.1` at runtime. These are not installed by default on most Linux distributions.

The packaging workflow addresses this with two mechanisms:

1. **RPATH** — The CMake flag `-DCMAKE_INSTALL_RPATH="\$ORIGIN/lib"` tells the dynamic linker to look for shared libraries in a `lib/` directory next to the executable.
2. **`bundle-deps`** — The `package_linux.sh` script calls `bundle-deps`, a Go tool that runs `ldd` on the executable, filters out standard system libraries, and copies the remaining dependencies into the `lib/` directory.

If you statically link libc++ (see [Building for Linux](building-linux.md)), `bundle-deps` is not needed.

> **Tip:** On development machines, the required libraries are usually already installed globally, so the executable works without bundling. However, on end-user machines these libraries are likely missing — always use `bundle-deps` for shipping packages.

For more details on static linking and shared library options, see [Building for Linux](building-linux.md).

## macOS

Packaging script for macOS is not yet implemented. It will be added in a future update.

## Emscripten / WebAssembly

```sh
# 1. Bootstrap the pipeline tools
./tools/script/bootstrap.sh

# 2. Build assets
./tools/script/assetbuild.sh

# 3. (Optional) Add Ninja to PATH if not installed system-wide
PATH=$PATH:./build/tools

# 4. Activate emsdk
source path/to/emsdk/emsdk_env.sh

# 5. Generate Ninja files (Release)
cmake -Bbuild/emscripten_release -H. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake

# 6. Build
ninja -C build/emscripten_release

# 7. Create the shipping package
./tools/script/package_emscripten.sh
```

The packaging script copies the Emscripten build output (`.js`, `.wasm`) and `index.html` into `build/<app>/shipping/web/`. The asset build in step 2 has already produced `content.idx` and `content.pak` in the same directory. The final shipping directory contains:

```
build/<app>/shipping/web/
├── <app>.js
├── <app>.wasm
├── content.idx
├── content.pak
└── index.html
```

### Running Locally

To run the application locally in a browser:

```sh
# Linux, macOS
emrun --browser chrome ./build/feature_showcase/shipping/web/index.html

# Windows
emrun.bat --browser chrome ./build/feature_showcase/shipping/web/index.html
```

### Deploying to a Web Server

Upload the contents of the shipping directory to any web server:

```sh
scp build/<app>/shipping/web/* user@example.com:/var/www/html/game/
```

Then open `index.html` in a browser to play the game. The web server must serve `.wasm` files with the `application/wasm` MIME type.

### Emscripten-Specific Constraints

- **Threading (pthread):** By default, Pomdog runs in single-threaded mode on Emscripten. To enable pthread support, set the CMake option `POMDOG_ENABLE_EMSCRIPTEN_PTHREAD` to `1`. This requires the web server to send the `Cross-Origin-Opener-Policy: same-origin` and `Cross-Origin-Embedder-Policy: require-corp` headers to enable `SharedArrayBuffer`.
- **Memory growth:** Games often require large amounts of memory. Pomdog's example applications use `-s ALLOW_MEMORY_GROWTH` by default to allow the WebAssembly heap to grow dynamically. Custom applications should also set this flag if needed (in CMake: `"SHELL:-s ALLOW_MEMORY_GROWTH"`).
- **Network:** Direct TCP/UDP socket communication is not available in browser environments. Emscripten's network layer is currently a stub implementation in Pomdog.

## Writing a Packaging Script for Your Own Application

The built-in packaging scripts (`package_windows.sh`, `package_linux.sh`) are written for the example applications and cannot be used directly for custom applications. However, the scripts are intentionally simple — they copy the executable and archive files into a shipping directory, and on Linux additionally run `bundle-deps`.

Use the existing scripts as a reference and create a similar one for your application, adjusting paths and filenames as needed.
