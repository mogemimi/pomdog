# Pomdog

[![Join the chat at https://gitter.im/mogemimi/pomdog](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mogemimi/pomdog?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Pomdog is an experimental game engine, written in C++17 or later, developed as a hobby.
It is open source and distributed under the [MIT License](http://opensource.org/licenses/MIT).

* [Homepage](http://enginetrouble.net/pomdog)
* [GitHub](https://github.com/mogemimi/pomdog)
* [Documentation](https://github.com/mogemimi/pomdog/docs)
* [Wiki](https://github.com/mogemimi/pomdog/wiki)

Feedback, issues and pull requests are always welcome!

* Bugs and issues can be raised in [Issues on GitHub](https://github.com/mogemimi/pomdog/issues).
* Please feel free to ping me on [Gitter chat room](https://gitter.im/mogemimi/pomdog) if you have any questions or feature requests.
* You can track feature requests and the progress of its features/issues on [Trello board](https://trello.com/b/lqd3nwrK/pomdog-game-engine). So feel free to add some comments or vote for your favorite feature requests.

## Supported platforms

Pomdog is available and supported on the following platforms:

* Mac OS X 10.11 and later [![Build Status](https://travis-ci.org/mogemimi/pomdog.svg?branch=master)](https://travis-ci.org/mogemimi/pomdog)
* Windows 10 and later [![Build Status](https://ci.appveyor.com/api/projects/status/hywx5fmkrk7gpdpl/branch/master?svg=true)](https://ci.appveyor.com/project/mogemimi/pomdog/branch/master)
* Linux (Ubuntu 18.04 LTS and Arch Linux) [![wercker status](https://app.wercker.com/status/e7bab185762fa586284d832580e89561/s/master "wercker status")](https://app.wercker.com/project/bykey/e7bab185762fa586284d832580e89561)

Please refer to [Architecture](https://github.com/mogemimi/pomdog/wiki/Architecture) in the wiki for more details about the supported platforms and backends such as OpenGL, Metal, Vulkan, DirectX 11 and 12.

## How to build

1. Clone the repository
2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init
    ```

3. To generate all project files (.xcodeproj or .vcxproj), run the following command:

    ```shell
    cd path/to/pomdog

    # Generate Xcode project
    cmake -Bbuild.cmake -H. -G Xcode

    # Generate projects for Visual Studio 2017
    cmake -Bbuild.cmake -H. -G "Visual Studio 15"

    # Generate Ninja files
    cmake -Bbuild.cmake -H. -G Ninja
    ```

After generating, open the .xcodeproj or .vcxproj in Xcode/Visual Studio.
If you need further information about building runtime libraries and developing apps, please visit:

* [Building Runtime Libraries on Linux](https://github.com/mogemimi/pomdog/wiki/Building-Runtime-Libraries-on-Linux)
* [Building Runtime Libraries on Mac OS X](https://github.com/mogemimi/pomdog/wiki/Building-Runtime-Libraries-on-Mac-OS-X)
* [Building Runtime Libraries on Windows](https://github.com/mogemimi/pomdog/wiki/Building-Runtime-Libraries-on-Windows)
* [Developing on Mac OS X](https://github.com/mogemimi/pomdog/wiki/Developing-on-Mac-OS-X)
* [Developing on Windows (Visual Studio 2017)](https://github.com/mogemimi/pomdog/wiki/Developing-on-Windows-(Visual-Studio-2017))

## Getting started

To create a new project with Pomdog, run `tools/quickstart.py`:

```shell
cd path/to/your-directory
python pomdog/tools/quickstart.py
```

For more information, please see [Getting Started](https://github.com/mogemimi/pomdog/wiki/Getting-Started).
