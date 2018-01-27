# QuickStart

## Prerequisites

#### Runtime requirements

* Mac OS X 10.9+
* Windows 8+
* OpenGL 4, DirectX 11 or DirectX 12

#### Build requirements

* Python 2.7
* Clang 3.6 (for Linux)
* Xcode 6.3 and later
* Visual Studio 2015 and later

#### Pulling all dependencies using Git

Make sure git is installed.
From the root of your project directory, run:

```sh
cd QuickStart
git clone --depth=1 https://github.com/mogemimi/pomdog.git ThirdParty/pomdog
git clone --depth=1 https://github.com/mogemimi/pomdog-third-party.git ThirdParty/pomdog/third-party
git clone --depth=1 https://chromium.googlesource.com/external/gyp.git Tools/gyp
```

## How to build

### Building under Linux

**1. Generating Makefiles**

```sh
python Tools/gyp/gyp_main.py examples/QuickStart/QuickStart.gyp --depth=. \
  -f make --generator-output=build.makefiles -Dcomponent=static_library
```

**2. Building (Release/Debug)**

```sh
export CC=clang
export CXX=clang++
export LINK=clang++
export CXXFLAGS="-std=c++17 -stdlib=libc++"
export LDFLAGS="-stdlib=libc++"
make -C build.makefiles
```

To build in release mode, use `BUILDTYPE` option:

```sh
make -C build.makefiles BUILDTYPE="Release"
```

**3. Running app**

```sh
# Copy asset files to output directory
cp -R examples/QuickStart/Content/ build.makefiles/out/Release/Content

# Run
build.makefiles/out/Release/QuickStart
```

### Building under Mac OS X and Xcode

**1. Generating the Xcode project file**

```sh
python Tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f xcode --generator-output=Build.xcodefiles
```

**2. Building (Release/Debug)**

```sh
xcodebuild -project Build.xcodefiles/QuickStart.xcodeproj
```

To build in release mode, use `-configuration` option:

```sh
xcodebuild -project Build.xcodefiles/QuickStart.xcodeproj -configuration Release
```

**3. Running app**

```sh
open build/Release/QuickStart.app
```

### Building under Visual Studio 2015

Generate the Visual Studio project files:

**PowerShell**

```powershell
python Tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f msvs `
    -G msvs_version=2015 --generator-output=Build.msvs
```

**Git Bash (MinGW)**

```sh
python Tools/gyp/gyp_main.py QuickStart.gyp --depth=. -f msvs \
    -G msvs_version=2015 --generator-output=Build.msvs
```

Open `Build.msvs/QuickStart.sln` in Visual Studio and build your app.
To run your app, change QuickStart project property to the following
at `Configuration Properties > Debugging > Working Directory` in Visual Studio:

|||
|:----|:----|
|Working Directory|`$(ProjectDir)..`|
