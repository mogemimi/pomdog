# Pomdog

[![Join the chat at https://gitter.im/mogemimi/pomdog](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mogemimi/pomdog?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Pomdog is an experimental game engine, written in C++14 or later, developed as a hobby.
It's open source and distributed under the [MIT license](http://opensource.org/licenses/mit-license.php).

* [Homepage](http://enginetrouble.net/pomdog)
* [GitHub](https://github.com/mogemimi/pomdog)
* [Wiki](https://github.com/mogemimi/pomdog/wiki)
* [Trello](https://trello.com/b/lqd3nwrK/pomdog-game-engine)

|Windows|Mac OS X|
|:------|:-------|
|[![Build status](https://ci.appveyor.com/api/projects/status/hywx5fmkrk7gpdpl/branch/master?svg=true)](https://ci.appveyor.com/project/mogemimi/pomdog/branch/master)|[![Build Status](https://travis-ci.org/mogemimi/pomdog.svg?branch=master)](https://travis-ci.org/mogemimi/pomdog)|

## Requirements

* Mac OS X 10.9+
* Windows 8+
* Direct3D 11 and 11.1
* Direct3D 12 (preliminary)
* OpenGL 4

### Build requirements

* Python 2.7+
* Xcode 6.3+ (Apple LLVM 6.1 based on Clang 3.6)
* Visual Studio 2015+

>**Note: Support Windows 8 Desktop and Windows Store**  
>Pomdog will support development of Windows 8 Desktop and Windows Store apps;
>however, Visual C++ Compiler (2015 CTP 6) doesn't completely support C++14.

## How to build

* [Developing on Mac OS X](https://github.com/mogemimi/pomdog/wiki/Developing-on-Mac-OS-X)
* [Developing on Windows (Visual Studio 2015)](https://github.com/mogemimi/pomdog/wiki/Developing-on-Windows-(Visual-Studio-2015))

## Getting started

To get started with new project, run ```tools/quickstart.py```:

```shell
cd ~/your-projects
python pomdog/tools/quickstart.py
```
