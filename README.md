# Pomdog

[![Join the chat at https://gitter.im/mogemimi/pomdog](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mogemimi/pomdog?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Pomdog is an experimental game engine, written in C++23 or later, developed as a hobby.
It is open source and distributed under the [MIT License](http://opensource.org/licenses/MIT).

* [Homepage](http://enginetrouble.net/pomdog)
* [GitHub](https://github.com/mogemimi/pomdog)
* [Documentation](https://github.com/mogemimi/pomdog/tree/main/docs)
* [Wiki](https://github.com/mogemimi/pomdog/wiki)

Feedback, issues and pull requests are always welcome!

* Bugs and issues can be raised in [Issues on GitHub](https://github.com/mogemimi/pomdog/issues).
* Please feel free to ping me on [Gitter chat room](https://gitter.im/mogemimi/pomdog) if you have any questions or feature requests.
* You can track feature requests and the progress of its features/issues on [Trello board](https://trello.com/b/lqd3nwrK/pomdog-game-engine). So feel free to add some comments or vote for your favorite feature requests.

## Supported platforms

Pomdog is available and supported on the following platforms:

* macOS 10.14 and later [![Build on macOS](https://github.com/mogemimi/pomdog/actions/workflows/build-macos.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-macos.yml)
* Windows 10 and later [![Build on Windows](https://github.com/mogemimi/pomdog/actions/workflows/build-windows.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-windows.yml)
* Linux (Ubuntu 20.04 and Arch Linux) [![Build on Linux](https://github.com/mogemimi/pomdog/actions/workflows/build-linux.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-linux.yml)
* Emscripten and WebAssembly (Wasm) [![Build on Emscripten](https://github.com/mogemimi/pomdog/actions/workflows/build-emscripten.yml/badge.svg)](https://github.com/mogemimi/pomdog/actions/workflows/build-emscripten.yml)

Please refer to [Architecture](docs/Architecture.md) in the wiki for more details about the supported platforms and backends such as OpenGL, Metal, Vulkan, DirectX 11 and 12.

## How to build

1. Clone the repository
2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init --recursive
    ```

3. To generate all project files (.xcodeproj or .vcxproj), run the following command:

    ```shell
    cd path/to/pomdog

    # Generate Xcode project
    cmake -Bbuild/macos -H. -G Xcode -DCMAKE_XCODE_GENERATE_SCHEME=ON

    # Generate projects for Visual Studio 2022
    cmake -Bbuild/windows -H. -G "Visual Studio 17"

    # Generate Ninja files for Linux
    cmake -Bbuild/linux -H. -G Ninja

    # Generate Ninja files for Emscripten
    cmake -Bbuild/emscripten -H. -G Ninja
    ```

After generating, open the .xcodeproj or .vcxproj in Xcode/Visual Studio.
If you need further information about building runtime libraries and developing apps, please visit:

* [Running the Tests](docs/Running-the-Tests.md)
* [Setting Up Development Environment on Ubuntu](docs/Setting-Up-Development-Environment-on-Ubuntu.md)

## Getting started

To create a new project with Pomdog, run `tools/quickstart/main.go`:

```sh
cd pomdog/tools/cmd/quickstart
go build

cd path/to/your/directory
./pomdog/tools/cmd/quickstart/quickstart
```

For more information, please see [Getting Started](docs/Getting-Started.md).
