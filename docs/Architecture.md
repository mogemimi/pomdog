# Architecture

Pomdog can be used to make either 2D or 3D games for Windows, macOS and Linux platforms.
Even if you don't know the platform-specific API such as DirectX, Metal, OpenGL or Vulkan, you can build the best graphics pipeline for the games you develop.
In addition to basic features such as graphics, input, network, sound, and asynchronous modules required for game development, experimental libraries including ECS, post-process effects, animation and particle simulation are also included.
If necessary, you can use it as a minimal lightweight game engine by removing these experimental features.
It is open source, so you can customize it as much as you want for the game you want to make.

## Features

Here is a list of some of core features of Pomdog. These are:

- Multi platform (Windows, macOS and Linux).
- Providing modern graphics API and supporting multiple backends (work on DirectX 11, OpenGL 4, Metal).
- Supporting TCP, UDP and TLS networking.
- Supporting keyboard, mouse, gamepad and joystick.
- Audio Engine (implementation on XAudio2 and OpenAL).
- Signals and Slot.
- Promises, async/await.

The following features are experimentally supported:

- Primitives, billboards, sprites, lines and text batch rendering.
- Supporting several image formats (PNG, DDS, PNM/Netpbm, TrueType fonts, SVG, Animated GIF).
- 2D/3D particle system.
- 2D skeletal animation (blend trees, skined mesh).
- Post process effects (FXAA, chromatic aberration, retro CRT, fish eye, sepia tone).
- ECS (Entity component system).
- In-game editor GUI.
- MagicaVoxel importer/exporter.
- Tween/Easing functions.
- Functional reactive programming library.

## Supported backends

||Windows Desktop|macOS|Linux|
|:---|:---|:---|:----|
|OS Version|Windows 10 and later|macOS 10.12 and later|Ubuntu 19.04 LTS and later|
|Window System|Win32<br>Universal Windows Platform (WIP)|Cocoa<br>XQuartz (with restrictions)|X11|
|Graphics|Direct3D 11<br>OpenGL 4<br>Direct3D 12 (WIP)<br>Vulkan (WIP)|Metal<br>OpenGL 4<br>Vulkan (work on MoltenVK, WIP)|OpenGL 4<br>Vulkan (WIP)|
|Audio|XAudio 2|OpenAL<br>CoreAudio/AudioUnit Toolkit (WIP)|OpenAL|
|Gamepad|DirectInput<br>XInput (WIP)|IOKit (IOHIDManager)|Input Subsystem|
|Keyboard/Mouse|Raw Input<br>Win32|Cocoa|X11|
|Network|WinSock2|POSIX Socket|POSIX Socket|

## Dependencies

#### Tooling

- Golang

#### Build tools

- CMake
- Ninja

#### Compile-time and runtime dependencies

- zlib
- libpng
- GLEW (for Linux, X11 and Windows Desktop + OpenGL)
- Mbed TLS
- Mbed Crypto
- Nano SVG
- GIFLIB
- RapidJSON
- SDL_GameControllerDB
- stb
- UTF8-CPP

#### Dependencies to compile and run the unit tests

- Catch2
