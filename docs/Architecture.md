# Architecture

## Supported backends

||Windows Desktop|macOS|Linux|
|:---|:---|:---|:----|
|OS Version|Windows 10 and later|macOS 10.12 and later|Ubuntu 19.04 LTS and later|
|Window System|Win32<br>Universal Windows Platform (WIP)|Cocoa<br>XQuartz (with restrictions)|X11|
|Graphics|Direct3D 11<br>OpenGL 4<br>Direct3D 12 (WIP)|Metal<br>OpenGL 4|OpenGL 4<br>Vulkan (WIP)|
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
