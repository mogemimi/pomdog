# QuickStart

## Requirements

* Mac OS X 10.9+
* Xcode 6.0+ (Apple LLVM 6.0/Clang 3.5)
* OpenGL 4 and Direct3D 11.2/12
* Python 2.7+

## How to build

### Pulling all dependencies using Git

Make sure git is installed.
From the root of your project directory, run:

```bash
cd QuickStart
git clone https://github.com/mogemimi/pomdog.git Pomdog
git clone https://github.com/mogemimi/pomdog-third-party.git Pomdog/third-party
git clone https://chromium.googlesource.com/external/gyp.git Tools/gyp
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```bash
Tools/gyp/gyp Build/QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

You can also use `gyp` instead of `Tools/gyp/gyp`:

```bash
gyp Build/QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

For information on how to install gyp, see [How to Install GYP](https://github.com/mogemimi/pomdog/wiki/How-to-Install-GYP) on the wiki.

**2. Building (Release/Debug)**

```
xcodebuild -project Build.xcodefiles/Build/QuickStart.xcodeproj
```

To build in release mode, use `-configuration` option:

```bash
xcodebuild -project Build.xcodefiles/Build/QuickStart.xcodeproj -configuration Release
```

**3. Running app**

```bash
open Build/build/Release/QuickStart.app
```
