Pomdog
=======

## Pomdog is

* game engine for C++11 or later
* completely open source (distributed under the [MIT License](http://opensource.org/licenses/mit-license.php))

For more information, see

* [http://enginetrouble.net/pomdog](http://enginetrouble.net/pomdog)
* [https://github.com/mogemimi/pomdog](https://github.com/mogemimi/pomdog)

## Requirements

* Mac OS X 10.9
* Xcode 5
* OpenGL 4.0 or Direct3D 11
* Python 2.7+

>**Note: Support Windows 8 Desktop and Windows Store**  
>Pomdog will support development of Windows 8 Desktop and Windows Store apps; however, Visual C++ Compiler (Nov 2013 CTP) doesn't completely support C++11.

## How to build

### Prerequisite: Installing GYP

First, install GYP from https://chromium.googlesource.com/external/.  
Make sure git is installed.
From the root of your engine directory, run:  
```
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

```
$ cd tools/gyp
$ python setup.py install
```

### Building under Xcode (Apple LLVM Clang++)

From the root of your engine directory, run:  
```bash
gyp build/cocoa_test.gyp --depth=. -f xcode --generator-output=./build.xcodefiles/
xcodebuild -project build.xcodefiles/build/cocoa_test.xcodeproj
```
