# Pomdog

[![Join the chat at https://gitter.im/mogemimi/pomdog](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mogemimi/pomdog?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Pomdog is an experimental game engine, written in C++14 or later, developed as a hobby.
It is open source and distributed under the [MIT License](http://opensource.org/licenses/MIT).

* [Homepage](http://enginetrouble.net/pomdog)
* [GitHub](https://github.com/mogemimi/pomdog)
* [Wiki](https://github.com/mogemimi/pomdog/wiki)

Feedback, issues and pull requests are always welcome!

* Bugs and issues can be raised in [Issues on GitHub](https://github.com/mogemimi/pomdog/issues).
* Please feel free to ping me on [Gitter chat room](https://gitter.im/mogemimi/pomdog) if you have any questions or feature requests.
* You can track feature requests and the progress of its features/issues on [Trello board](https://trello.com/b/lqd3nwrK/pomdog-game-engine). So feel free to add some comments or vote for your favorite feature requests.

## Supported platforms

Pomdog is available and supported on the following platforms:

* Mac OS X 10.9 and later [![Build Status](https://travis-ci.org/mogemimi/pomdog.svg?branch=master)](https://travis-ci.org/mogemimi/pomdog)
* Windows 8 and later [![Build Status](https://ci.appveyor.com/api/projects/status/hywx5fmkrk7gpdpl/branch/master?svg=true)](https://ci.appveyor.com/project/mogemimi/pomdog/branch/master)

Please refer to [Architecture](https://github.com/mogemimi/pomdog/wiki/Architecture) in the wiki for more details about the supported platforms and backends such as OpenGL, DirectX 11 and 12.

## Getting started

To create a new project with Pomdog, run `tools/quickstart.py`:

```shell
git clone https://github.com/mogemimi/pomdog.git
python pomdog/tools/quickstart.py
```

## How to build

To automagically generate all project files (.xcodeproj or .vcxproj), run `tools/generate_projects.py`:

```shell
cd path/to/pomdog
python tools/generate_projects.py
```

After generating, open the .xcodeproj or .vcxproj in Xcode/Visual Studio.
For more infomation about building manually, please see:

* [Developing on Mac OS X](https://github.com/mogemimi/pomdog/wiki/Developing-on-Mac-OS-X)
* [Developing on Windows (Visual Studio 2015)](https://github.com/mogemimi/pomdog/wiki/Developing-on-Windows-(Visual-Studio-2015))
