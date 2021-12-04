# Developing Pomdog Game Engine

## Prerequisites

The following software is required in order to create and build the Pomdog game engine:

- [Git](https://www.git-scm.com/)
- [CMake](https://cmake.org/)
- For Linux:
  - [Ninja](https://ninja-build.org/)
  - Clang or GCC
  - For more details about installation of build requirements, please see [Setting Up Development Environment on Ubuntu](Setting-Up-Development-Environment-on-Ubuntu.md).
  - Docker Images: [Ubuntu](https://hub.docker.com/r/mogemimi/gamedev-ubuntu) and [Archlinux](https://hub.docker.com/r/mogemimi/gamedev-archlinux/)
- For Windows:
  - Visual Studio
- For macOS:
  - Xcode

## Build requirements

- CMake 3.20 and later
- Linux
  - Clang >= 11.0
  - GCC >= 8.3
- Windows
  - Visual Studio 2019 or newer
- macOS
  - Xcode 11.3 or newer

## Runtime requirements

- Linux
  - Ubuntu 20.04 or Archlinux
  - OpenGL 4.1+
  - OpenAL
- Windows
  - Windows 10
  - DirectX 11.3+
  - OpenGL 4.1+
- macOS
  - Mac OS X 10.14 and later
  - Metal
  - OpenGL 4.1+

## Clone the repo with Git

First, acquire the source code and all dependencies by cloning the git repository.

1. Clone the repository

    ```sh
    git clone https://github.com/mogemimi/pomdog.git
    ```

2. Make sure that submodules are checked out and up-to-date:

    ```shell
    git submodule update --init --recursive
    ```

## Build and run the unit tests

Second, you need to generate build files using CMake, build the sources, and run the tests.
For more details about building and testing, please see [Running the Tests](Running-the-Tests.md).
