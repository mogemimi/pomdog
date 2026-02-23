# Building for Linux

This page covers toolchain options, static linking, and shared library bundling for Linux builds.

For a quick start, see the [Linux section in Running the Tests](running-the-tests.md#linux).

## Building with a custom toolchain

The default build uses Clang with libc++. You can also use GCC or Clang with libstdc++.

### GCC

```sh
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Debug
```

### Clang with libstdc++

```sh
cmake -Bbuild/linux_debug -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libstdc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libstdc++" \
    -DCMAKE_BUILD_TYPE=Debug
```

> **Note:** Using GCC or Clang with libstdc++ avoids the `libc++.so.1` / `libc++abi.so.1` dependency issues described below, since libstdc++ is installed by default on most Linux distributions.

## Distributing binaries built with libc++

When you build with Clang and `-stdlib=libc++`, the resulting binary depends on `libc++.so.1` and `libc++abi.so.1` at runtime. These are not installed by default on most Linux distributions, so running the binary on another machine may produce:

```
error while loading shared libraries: libc++abi.so.1: cannot open shared object file: No such file or directory
```

There are two approaches to solve this: **statically linking libc++** or **bundling the shared libraries with RPATH**.

### Statically linking libc++

Statically link libc++ and libc++abi into the executable so no external `.so` files are needed:

```sh
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -Wl,-Bstatic -lc++ -lc++abi -Wl,-Bdynamic -lpthread -ldl" \
    -DCMAKE_BUILD_TYPE=Release
```

### Bundling shared libraries with RPATH

Alternatively, bundle the shared libraries alongside the executable and set RPATH so the dynamic linker finds them at runtime:

```sh
cmake -Bbuild/linux_release -H. -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -lc++abi" \
    -DCMAKE_INSTALL_RPATH="\$ORIGIN/lib" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_BUILD_TYPE=Release
```

Then copy the shared libraries next to the built binary:

```sh
mkdir -p build/linux_release/tests/lib
cp /usr/lib/x86_64-linux-gnu/libc++.so.1    build/linux_release/tests/lib/
cp /usr/lib/x86_64-linux-gnu/libc++abi.so.1 build/linux_release/tests/lib/
```

The resulting directory layout:

```
build/linux_release/
  tests/pomdog_tests
  tests/lib/
    libc++.so.1
    libc++abi.so.1
```

### Using bundle-deps to automate shared library bundling

Manually copying libraries can be tedious and error-prone — transitive dependencies like `libsndio.so.7` (pulled in by OpenAL) are easy to miss.

The `bundle-deps` tool automates this by running `ldd` on the binary, filtering out standard system libraries, and copying the rest into the output directory.

Build the tool:

```sh
cd tools/cmd/bundle-deps
go build -o ../../../build/tools/bundle-deps
cd ../../..
```

Run it on a single binary:

```sh
./build/tools/bundle-deps -v \
    -o build/linux_release/tests/lib \
    build/linux_release/tests/pomdog_tests
```

Or use the provided script to bundle dependencies for all examples and tests at once:

```sh
./tools/script/bundle_deps_linux_release.sh
```

This script processes the following targets:

- `build/linux_release/examples/feature_showcase`
- `build/linux_release/examples/pong`
- `build/linux_release/examples/quickstart`
- `build/linux_release/tests`

For more details on the tool, see [tools/cmd/bundle-deps/README.md](../tools/cmd/bundle-deps/README.md).
