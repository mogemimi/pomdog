# Pomdog

Pomdog is an experimental game engine written in C++23 or later, developed as a hobby.
It is open source and distributed under the [MIT License](http://opensource.org/licenses/MIT).

* [GitHub](https://github.com/mogemimi/pomdog)
* [Documentation](docs/README.md)
* [Homepage](http://enginetrouble.net/pomdog)

Feedback, issues, and pull requests are always welcome!  
If you find a bug or have a suggestion, feel free to open an issue on [GitHub](https://github.com/mogemimi/pomdog/issues).

## Supported platforms

|Platform|Status|
|:---|:---|
|Windows 11 and later|[![Build on Windows](https://github.com/mogemimi/pomdog/actions/workflows/build-windows.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-windows.yml)|
|macOS 11.0 and later|[![Build on macOS](https://github.com/mogemimi/pomdog/actions/workflows/build-macos.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-macos.yml)|
|Linux (Ubuntu 24.04 and Arch Linux)|[![Build on Linux](https://github.com/mogemimi/pomdog/actions/workflows/build-linux.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-linux.yml)|
|Emscripten / WebAssembly|[![Build on Emscripten](https://github.com/mogemimi/pomdog/actions/workflows/build-emscripten.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-emscripten.yml)|

For more details on supported backends (Direct3D, Metal, Vulkan, OpenGL, etc.), see [Architecture](docs/architecture.md).

## Documentation

* **If you'd like to make games using Pomdog**, check out [Getting Started](docs/getting-started.md).
* **If you'd like to work on the engine itself**, see [Developing Pomdog Game Engine](docs/developing-pomdog-game-engine.md).

For everything else, you can browse the full [docs](docs/README.md) directory.

## Quick start

1. Clone the repository and initialize submodules:

    ```sh
    git clone https://github.com/mogemimi/pomdog.git
    cd pomdog
    git submodule update --init --recursive
    ```

2. Run the bootstrap script to set up the asset pipeline tools:

    ```sh
    ./tools/script/bootstrap.sh
    ```

3. Build assets for the example applications:

    ```sh
    ./tools/script/assetbuild.sh
    ```

    The examples require pre-built assets (shaders, fonts, etc.) to run. For details on the asset pipeline, see [Asset Pipeline and Runtime](docs/asset-pipeline-and-runtime.md).

4. Generate project files and build:

    ```sh
    # Windows (Visual Studio 2026)
    cmake -Bbuild/windows -H. -G "Visual Studio 18"
    cmake --build build/windows --config Debug

    # macOS (Xcode)
    cmake -Bbuild/macos -H. -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON
    xcodebuild -project build/macos/pomdog.xcodeproj -configuration Debug

    # Linux (Ninja)
    cmake -Bbuild/linux -H. -G Ninja -DCMAKE_BUILD_TYPE=Debug
    ninja -C build/linux
    ```

For more details, see [Running the Tests](docs/running-the-tests.md).
