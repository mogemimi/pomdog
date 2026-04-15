# quickstart

A minimal Pomdog game project that demonstrates basic engine features: window creation, texture loading, shader compilation, and rendering a textured quad with rotation.

This project also serves as the template for the [quickstart tool](../../tools/cmd/quickstart/README.md), which copies and adapts these files to create new game projects.

## Getting Started

To create a new game project based on this template, see [Getting Started](../../docs/getting-started.md).

## Building

For build instructions on each platform (Windows, Linux, macOS, Emscripten), see [Running the Tests](../../docs/running-the-tests.md) — the same CMake workflow applies to this example project.

## Asset Pipeline

This example uses the Pomdog asset pipeline to compile shaders, copy textures, and produce archive files. For details, see [Asset Pipeline and Runtime](../../docs/asset-pipeline-and-runtime.md).

## Shipping

To create a distributable package, see [Shipping](../../docs/shipping.md).


To develop your application on Xcode, please open `quickstart.xcodeproj` in Xcode.

### Building under Visual Studio 2022

```sh
# Git Bash (MinGW)
cd path/to/quickstart

# Generate projects for Visual Studio 2022 to the 'build' directory
cmake -Bbuild -H. -G "Visual Studio 17"

# Building projects using CMake and MSBuild
cmake --build build --config Debug

# To run your application, you can use the following
./build/Debug/quickstart
```

To build in release mode, use `--config` option:

```sh
cmake --build build --config Release
```

To develop your application on Visual Studio, please open `quickstart.sln` in Visual Studio.
