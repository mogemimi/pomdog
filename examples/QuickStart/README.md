# QuickStart

## Prerequisites

#### Runtime requirements

* Mac OS X 10.9+
* Windows 8+
* OpenGL 4, DirectX 11 or DirectX 12

#### Build requirements

* Python 2.7
* Xcode 6.3 and later
* Visual Studio 2015 and later

#### Pulling all dependencies using Git

Make sure git is installed.
From the root of your project directory, run:

```shell
cd QuickStart
git clone --depth=1 https://github.com/mogemimi/pomdog.git Pomdog
git clone --depth=1 https://github.com/mogemimi/pomdog-third-party.git Pomdog/third-party
git clone --depth=1 https://chromium.googlesource.com/external/gyp.git Pomdog/tools/gyp
```

## How to build

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```shell
python Pomdog/tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

You can also use `gyp` instead of `python Pomdog/tools/gyp/gyp_main.py`:

```shell
gyp QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

For information on how to install gyp, please see [How to Install GYP](https://github.com/mogemimi/pomdog/wiki/How-to-Install-GYP) on the GitHub wiki.

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
python Pomdog/tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f msvs \
  -G msvs_version=2015 \
  --generator-output=Build.msvs \
  -Dcomponent=static_library
```

Open `Build.msvs/QuickStart.sln` in Visual Studio and build your app.
To run your app, change QuickStart project property to the following
at `Configuration Properties > Debugging > Working Directory` in Visual Studio:

|||
|:----|:----|
|Working Directory|`$(ProjectDir)..\..`|
