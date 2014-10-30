# QuickStart

## Requirements

* Mac OS X 10.9+
* Xcode 5.1+ (Apple LLVM 5.1/Clang 3.4)
* OpenGL 4 and Direct3D 11.2/12
* Python 2.7+

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

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```bash
gyp Build/QuickStart.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
```

**2. Building (Release/Debug)**

```
xcodebuild -project build.xcodefiles/Build/QuickStart.xcodeproj
```

To build in release mode, use `-configuration` option:

```bash
xcodebuild -project build.xcodefiles/Build/QuickStart.xcodeproj -configuration Release
```

**3. Running app**

```bash
open Build/build/Release/QuickStart.app
```
