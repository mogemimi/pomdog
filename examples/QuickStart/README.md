# QuickStart

## Requirements

* Mac OS X 10.9+ and Windows 8+
* Xcode 6.0+ (Apple LLVM 6.0/Clang 3.5)
* Visual Studio 2015 (CTP6 and later)
* OpenGL 4, Direct3D 11 or Direct3D 12
* Python 2.7+

## How to build

### Pulling all dependencies using Git

Make sure git is installed.
From the root of your project directory, run:

```shell
cd QuickStart
git clone https://github.com/mogemimi/pomdog.git Pomdog
git clone https://github.com/mogemimi/pomdog-third-party.git Pomdog/third-party
git clone https://chromium.googlesource.com/external/gyp.git Tools/gyp
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```shell
python Tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

You can also use `gyp` instead of `python Tools/gyp/gyp_main.py`:

```shell
gyp QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

For information on how to install gyp, see [How to Install GYP](https://github.com/mogemimi/pomdog/wiki/How-to-Install-GYP) on the wiki.

**2. Building (Release/Debug)**

```shell
xcodebuild -project Build.xcodefiles/QuickStart.xcodeproj
```

To build in release mode, use `-configuration` option:

```shell
xcodebuild -project Build.xcodefiles/QuickStart.xcodeproj -configuration Release
```

**3. Running app**

```shell
open build/Release/QuickStart.app
```

### Building under Win32 and Visual Studio

Generate the Visual Studio project file:

```shell
Tools/gyp/gyp QuickStart.gyp --depth=. -f msvs \
  -G msvs_version=2015 \
  --generator-output=Build.msvs \
  -Dcomponent=static_library
```

Open `Build.msvs/QuickStart.sln` in Visual Studio and build your app.
To run your app, change QuickStart project property to the following
at `Configuration Properties > Debugging > Working Directory` in Visual Studio:

|:----|:----|
|Working Directory|`$(ProjectDir)..\..`|
