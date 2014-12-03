Pomdog
=======

Pomdog is an experimental game engine, written in C++14 or later, developed as a hobby.
It's open source and distributed under the [MIT License](http://opensource.org/licenses/mit-license.php).

[Homepage](http://enginetrouble.net/pomdog)  
[GitHub](https://github.com/mogemimi/pomdog)

## Requirements

* Mac OS X 10.9+
* OpenGL 4
* Windows 8+
* Direct3D 11+ (included DirectX 12)

### Build Requirements

* Python 2.7+
* Xcode 6.0+ (Apple LLVM 6.0/Clang 3.5)
* Visual Studio 2015+

>**Note: Support Windows 8 Desktop and Windows Store**  
>Pomdog will support development of Windows 8 Desktop and Windows Store apps;
>however, Visual C++ Compiler (2015 Preview) doesn't completely support C++11.

## How to build

### Pulling all dependencies using Git

Make sure git is installed.
From the root of your engine directory, run:

```bash
cd pomdog
git clone https://github.com/mogemimi/pomdog-third-party.git third-party
git clone https://chromium.googlesource.com/external/gyp.git tools/gyp
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```bash
tools/gyp/gyp build/TestApp.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
```

You can also use `gyp` instead of `tools/gyp/gyp`:

```bash
gyp build/TestApp.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
```

For information on how to install gyp, see [How to Install GYP](https://github.com/mogemimi/pomdog/wiki/How-to-Install-GYP) on the wiki.

**2. Building (Release/Debug)**

```
xcodebuild -project build.xcodefiles/build/TestApp.xcodeproj
```

To build in release mode, use `-configuration` option:

```bash
xcodebuild -project build.xcodefiles/build/TestApp.xcodeproj -configuration Release
```

**3. Running app**

```bash
open build/build/Release/TestApp.app
```
