# archive-ninja-gen

A code generator that produces [Ninja](https://ninja-build.org/) build files for the asset archive packaging step. It generates build rules that invoke `archive-content` to pack all processed asset files into shipping archive files (`content.idx` + `content.pak`).

In the Pomdog asset pipeline, the final step is to pack all content files — compiled shaders, fonts, textures, sounds, etc. — into a single binary archive for each target platform. Previously, `archive-content` was called directly from `assetbuild.sh`, which meant the archive was rebuilt from scratch on every run even when no input files had changed. By wrapping this step in a Ninja build rule with depfile support, the archive is only rebuilt when one of its constituent files changes, enabling true incremental builds for the entire asset pipeline.

## Features

- **Per-platform output**: Generates build rules for each platform (windows, macos, linux, web) in a single Ninja file
- **Incremental builds via depfile**: `archive-content` emits a GCC-style depfile listing every packed file, so Ninja rebuilds the archive only when an input file changes
- **Implicit outputs**: Both `content.idx` and `content.pak` are tracked as build outputs
- **Recipe-driven**: Accepts the same `archive*.toml` recipe files used by `archive-content`

## Installation

```sh
cd tools/cmd/archive-ninja-gen
go build -o ../../../build/tools/archive-ninja-gen
```

## Usage

```sh
archive-ninja-gen [options] <recipe.toml> [recipe2.toml ...]
```

### Options

| Option | Description |
|--------|-------------|
| `-outninja <path>` | Output Ninja build file path |
| `-contentdir <path>` | Root content directory containing the processed asset files |
| `-outdir <path>` | Output shipping directory (`windows/`, `macos/`, `linux/`, and `web/` subdirectories are created) |
| `-intdir <path>` | Intermediate directory for depfiles and debug output |
| `-tooldir <path>` | Directory containing the `archive-content` executable |
| `-dep-subninja <path>` | Path to another `.ninja` file whose build outputs should be added as order-only dependencies (repeatable) |

Positional arguments are paths to `archive*.toml` recipe files.

### Dependency Ordering with `-dep-subninja`

The archive step must run **after** all shader compilation and file copying have finished, because it packs those outputs into the final archive. However, since each phase lives in its own subninja file, Ninja has no way to infer this ordering automatically.

The `-dep-subninja` flag solves this by parsing the specified `.ninja` file, extracting all build output paths, and injecting them as [order-only dependencies](https://ninja-build.org/manual.html#ref_dependencies) (`||`) on the archive build edges. This ensures Ninja finishes building those outputs before starting the archive step, without creating a hard rebuild dependency (i.e. the archive is not rebuilt just because a shader was recompiled — only when the depfile detects actual content changes).

The flag is repeatable. Typical usage passes all shader and copy ninja files:

```sh
./build/tools/archive-ninja-gen \
    -outninja ./build/quickstart/archivebuild/build.ninja \
    -contentdir ./build/quickstart/content \
    -outdir ./build/quickstart/shipping \
    -intdir ./build/quickstart/archivebuild \
    -tooldir ./build/tools \
    -dep-subninja ./build/quickstart/shaderbuild/shaders_pomdog.ninja \
    -dep-subninja ./build/quickstart/shaderbuild/shaders_app.ninja \
    -dep-subninja ./build/quickstart/copybuild/copy_pomdog.ninja \
    -dep-subninja ./build/quickstart/copybuild/copy_app.ninja \
    ./assets/archive/archive_fonts.toml \
    ./build/quickstart/archive/build/archive_shaders_pomdog.toml \
    ./build/quickstart/archive/build/archive_shaders_quickstart.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

Without `-dep-subninja`, the archive build edge might start before shaders finish compiling, leading to missing or stale files in the archive.

### Examples

Generate Ninja build file for the quickstart example:

```sh
./build/tools/archive-ninja-gen \
    -outninja ./build/quickstart/archivebuild/build.ninja \
    -contentdir ./build/quickstart/content \
    -outdir ./build/quickstart/shipping \
    -intdir ./build/quickstart/archivebuild \
    -tooldir ./build/tools \
    ./assets/archive/archive_fonts.toml \
    ./build/quickstart/archive/build/archive_shaders_pomdog.toml \
    ./build/quickstart/archive/build/archive_shaders_quickstart.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

## Output Directory Structure

```
<outdir>/
  windows/
    content.idx       # FlatBuffers index for Windows
    content.pak       # Binary archive for Windows
  macos/
    content.idx       # FlatBuffers index for macOS
    content.pak       # Binary archive for macOS
  linux/
    content.idx       # FlatBuffers index for Linux
    content.pak       # Binary archive for Linux
  web/
    content.idx       # FlatBuffers index for Emscripten / WebGL
    content.pak       # Binary archive for Emscripten / WebGL
<intdir>/
  windows/
    content.idx-debug # Debug index with human-readable paths (development only)
  macos/
    content.idx-debug # Debug index for macOS (development only)
  linux/
    content.idx-debug # Debug index for Linux (development only)
  web/
    content.idx-debug # Debug index for web platform (development only)
  windows.d           # Ninja depfile listing all packed files (Windows)
  macos.d             # Ninja depfile listing all packed files (macOS)
  linux.d             # Ninja depfile listing all packed files (Linux)
  web.d               # Ninja depfile listing all packed files (web)
```
