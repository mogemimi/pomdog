Pomdog
=======

Pomdog is an experimental game engine, written in C++14 or later, developed as a hobby.
It's open source and distributed under the [MIT License](http://opensource.org/licenses/mit-license.php).

[Homepage](http://enginetrouble.net/pomdog)  
[GitHub](https://github.com/mogemimi/pomdog)

## Requirements

* Mac OS X 10.9+
* Xcode 6.0+ (Apple LLVM 6.0/Clang 3.5)
* OpenGL 4 and Direct3D 11.2/12
* Python 2.7+

>**Note: Support Windows 8 Desktop and Windows Store**  
>Pomdog will support development of Windows 8 Desktop and Windows Store apps; however, Visual C++ Compiler (Nov 2013 CTP) doesn't completely support C++11.

## How to build

### Prerequisite: Installing GYP

First, install GYP from https://chromium.googlesource.com/external/.  
Make sure git is installed.
From the root of your engine directory, run:  
```bash
git clone https://chromium.googlesource.com/external/gyp.git tools/gyp
```

Second, run setup.py.

**Linux and Mac OS X**

To install globally with gyp:

```bash
cd tools/gyp
[sudo] python setup.py install
```

**Windows**

On Windows systems you can do:

```bash
cd tools/gyp
python setup.py install
```

### Pulling all dependencies using Git

From the root of your engine directory, run:  
```bash
git clone https://github.com/mogemimi/pomdog-third-party.git third-party
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```bash
gyp build/TestApp.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
```

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
