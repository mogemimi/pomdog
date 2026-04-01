# Deploying

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
./tools/script/deploy_windows.sh
```

The deploy script copies the Release `.exe` and the desktop archive files (`content.idx`, `content.pak`) into `build/<app>/shipping/windows/`:

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
./tools/script/deploy_linux.sh
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

The deploy workflow addresses this with two mechanisms:

1. **RPATH** — The CMake flag `-DCMAKE_INSTALL_RPATH="\$ORIGIN/lib"` tells the dynamic linker to look for shared libraries in a `lib/` directory next to the executable.
2. **`bundle-deps`** — The `deploy_linux.sh` script calls `bundle-deps`, a Go tool that runs `ldd` on the executable, filters out standard system libraries, and copies the remaining dependencies into the `lib/` directory.

If you statically link libc++ (see [Building for Linux](building-linux.md)), `bundle-deps` is not needed.

> **Tip:** On development machines, the required libraries are usually already installed globally, so the executable works without bundling. However, on end-user machines these libraries are likely missing — always use `bundle-deps` for shipping packages.

For more details on static linking and shared library options, see [Building for Linux](building-linux.md).

## macOS

Deploy script for macOS is not yet implemented. It will be added in a future update.

## Emscripten / WebAssembly

Deploy script for Emscripten is not yet implemented. Currently, the asset build produces `shipping/web/content.idx` and `shipping/web/content.pak`, but the script to bundle these with the Emscripten build output has not been written yet.

## Writing a Deploy Script for Your Own Application

The built-in deploy scripts (`deploy_windows.sh`, `deploy_linux.sh`) are written for the example applications and cannot be used directly for custom applications. However, the scripts are intentionally simple — they copy the executable and archive files into a shipping directory, and on Linux additionally run `bundle-deps`.

Use the existing scripts as a reference and create a similar one for your application, adjusting paths and filenames as needed.
