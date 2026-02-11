# Architecture

Pomdog is a cross-platform game engine for developing 2D and 3D games on Windows, macOS, Linux, and Emscripten (WebAssembly).
It provides a modern graphics API with multiple backends, so you can build your own optimized graphics pipelines without directly using platform-specific APIs such as Direct3D, Metal, OpenGL, or Vulkan.

In addition to core modules — graphics, input, networking, audio, and asynchronous primitives — Pomdog includes experimental libraries for ECS, post-processing effects, animation, and particle simulation.
These experimental features can be removed if you prefer a more minimal and lightweight engine.

Pomdog prioritizes keeping up with modern compilers, language standards, and platform trends. As a result, APIs are not guaranteed to be stable across versions. Backward compatibility is not maintained. Compiler warnings are enabled aggressively and treated as errors to take advantage of useful diagnostics.

The engine is open source and can be customized freely for your needs.

## Features

Core features:

- Multi-platform support (Windows, macOS, Linux, Emscripten/WebAssembly).
- Modern graphics API with multiple backends (Direct3D 11, OpenGL 4, Metal).
- Entity component system (ECS).
- TCP, UDP, and TLS networking.
- Input support for keyboard, mouse, gamepad, and joystick.
- Audio engine (XAudio2 on Windows, OpenAL on Linux/macOS).
- Signals and slots.

Experimental features:

- Promises, async/await.
- Primitives, billboards, sprites, lines, and text batch rendering.
- Image format support (PNG, DDS, PNM/Netpbm, TrueType fonts, SVG).
- 2D and 3D particle systems.
- 2D skeletal animation (blend trees, skinned mesh).
- Post-process effects (FXAA, chromatic aberration, retro CRT, fish eye, sepia tone).
- In-game editor GUI.
- MagicaVoxel importer/exporter.
- Tween/easing functions.

## Supported backends

||Windows|macOS|Linux|Emscripten|
|:---|:---|:---|:---|:---|
|OS Version|Windows 10 and later|macOS 11.0 and later|Ubuntu 24.04 and later<br>Arch Linux|Emscripten SDK (latest)|
|Window System|Win32|Cocoa|X11|HTML5 Canvas|
|Graphics|Direct3D 11<br>OpenGL 4<br>Direct3D 12 (WIP)<br>Vulkan (WIP)|Metal<br>OpenGL 4<br>Vulkan (MoltenVK, WIP)|OpenGL 4<br>Vulkan (WIP)|WebGL 2 (via Emscripten OpenGL)|
|Audio|XAudio 2|OpenAL|OpenAL|Web Audio (via Emscripten OpenAL)|
|Gamepad|DirectInput|IOKit (IOHIDManager)|Input Subsystem|Emscripten Gamepad API|
|Keyboard/Mouse|Raw Input / Win32|Cocoa|X11|Emscripten HTML5 Events|
|Network|WinSock2|POSIX Socket|POSIX Socket|Emscripten Networking (WIP)|

## Dependencies

#### Tooling

- Go
- CMake
- Ninja

#### Compile-time and runtime dependencies

- zlib
- libpng
- GLEW (for Linux and Windows + OpenGL)
- Mbed TLS
- Nano SVG
- RapidJSON
- SDL_GameControllerDB
- stb
- UTF8-CPP
- FlatBuffers
- fmt

#### Unit test dependencies

- Catch2
- doctest

For a complete list with licenses, see [Open Source Software Used in Pomdog](open-source-software-used-in-pomdog.md).
