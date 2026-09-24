# Building for Linux

For a quick start, see the [Linux section in Running the Tests](running-the-tests.md#linux).

## Building with a custom toolchain

The examples in this documentation use Clang with libc++. CMake otherwise uses the selected toolchain or system compiler; Pomdog does not force Clang. You can also use GCC or Clang with libstdc++.

### GCC

```sh
cmake -S . -B build/linux_debug -G Ninja \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Debug
```

### Clang with libstdc++

```sh
cmake -S . -B build/linux_debug -G Ninja \
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

Statically linking libc++ and libc++abi can remove those two runtime dependencies. Other shared dependencies, including platform libraries, may remain. Verify the resulting binary with `ldd`; the flags and available static archives depend on the toolchain:

```sh
cmake -S . -B build/linux_release -G Ninja \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++ -Wl,-Bstatic -lc++ -lc++abi -Wl,-Bdynamic -lpthread -ldl" \
    -DCMAKE_BUILD_TYPE=Release
```

### Bundling shared libraries with RPATH

Alternatively, bundle the shared libraries alongside the executable and set RPATH so the dynamic linker finds them at runtime:

```sh
cmake -S . -B build/linux_release -G Ninja \
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
build/linux_release/tests/
├── pomdog_tests            # executable file
└── lib/
    ├── libc++.so.1
    └── libc++abi.so.1
```

### Using bundle-deps to automate shared library bundling

Your development machine already has the libraries installed to build and run
the game. A player's machine may not. Shipping the executable alone can therefore
produce a missing-library error before the game starts.

Copying only the libraries your application links by name can also miss
dependencies. A library can require other libraries of its own: depending on how
OpenAL was built, it may need `libsndio.so.7`. Even if you include OpenAL, the game
can fail to start when that dependency is absent. Following those chains by hand
gets harder as the set of libraries changes.

Pomdog provides `bundle-deps` to automate that work. It runs `ldd` on the built
executable, filters out standard system libraries, and copies the remaining
dependencies into the package. Combine it with the RPATH configuration above so
the executable finds its bundled libraries. Test the package on the Linux systems
you support; bundling libraries does not remove system ABI requirements.

From the Pomdog checkout, build the tool:

```sh
cd tools/cmd/bundle-deps
go build -o ../../../build/tools/bundle-deps
cd ../../..
```

Bundle libraries for one executable built with the RPATH settings above:

```sh
./build/tools/bundle-deps -v \
    -o build/linux_release/tests/lib \
    build/linux_release/tests/pomdog_tests
```

To bundle libraries for the Release builds of all examples and tests, run:

```sh
./tools/script/bundle_deps_linux_release.sh
```

The script builds `bundle-deps` and writes a `lib/` directory beside each
executable under these directories:

- `build/linux_release/examples/feature_showcase`
- `build/linux_release/examples/pong`
- `build/linux_release/examples/quickstart`
- `build/linux_release/tests`

For more commands and exclusion options, see the
[bundle-deps README](../tools/cmd/bundle-deps/README.md). For the example applications'
packaging workflow, see [Shipping](shipping.md#linux).
