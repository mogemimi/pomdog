# bundle-deps

A command-line tool that bundles shared library dependencies for Linux binaries.

It runs `ldd` on a given binary, filters out standard system libraries, and copies the remaining shared libraries into a specified output directory. This is useful for distributing binaries built with Clang and libc++ to other Linux machines without requiring additional package installations.

## Installation

```sh
cd tools/cmd/bundle-deps
go build
```

## Usage

```sh
bundle-deps [options] <binary>
```

### Options

| Option | Description |
|--------|-------------|
| `-o <dir>` | Output directory for bundled shared libraries (required unless `-dry-run`) |
| `-exclude <prefixes>` | Comma-separated prefixes of libraries to exclude (see defaults below) |
| `-dry-run` | Print libraries that would be copied without actually copying |
| `-v` | Verbose output |

### Default excluded libraries

The following system library prefixes are excluded by default:

- `linux-vdso`, `ld-linux`
- `libc.so`, `libm.so`, `libpthread.so`, `libdl.so`, `librt.so`
- `libgcc_s.so`, `libstdc++.so`

### Examples

Bundle shared libraries for a release binary:

```sh
bundle-deps -v -o build/linux_release/tests/lib build/linux_release/tests/pomdog_tests
```

Preview which libraries would be bundled:

```sh
bundle-deps -dry-run build/linux_release/tests/pomdog_tests
```

Customize excluded libraries:

```sh
bundle-deps -exclude "linux-vdso,ld-linux,libc.so,libm.so" -o lib build/myapp
```

## Notes

- Symlinks are resolved; the actual file content is copied using the soname as the destination filename.
- Combine with `CMAKE_INSTALL_RPATH="\$ORIGIN/lib"` and `CMAKE_BUILD_WITH_INSTALL_RPATH=ON` so that the binary finds the bundled libraries at runtime.
- See [Running the Tests](../../../docs/running-the-tests.md) for full build instructions.
