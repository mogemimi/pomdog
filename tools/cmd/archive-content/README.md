# archive-content

An offline tool that packs multiple asset files into a single binary archive (`.pak`) with an accompanying FlatBuffers index (`.idx`). At runtime the engine maps file path hashes to byte ranges inside the `.pak`, enabling efficient O(log n) asset lookup without filesystem directory traversal.

The index stores only xxHash-64 hashes of file paths to minimise shipping binary size. A separate debug index (`.idx-debug`) retains the original path strings and the hash seed for diagnostic use; it is not included in release builds.

## Features

- **Single-file archive**: Concatenates all asset files into one `.pak` binary for fast sequential I/O
- **FlatBuffers index**: Generates a compact `.idx` with sorted hash keys for `LookupByKey()` binary search
- **Debug index**: Emits an `.idx-debug` with human-readable paths and the xxHash seed for hash verification
- **Platform filtering**: Supports `target_platforms` in the TOML recipe to include/exclude assets per platform
- **Hash collision detection**: Aborts with an error if two different file paths produce the same xxHash-64 value
- **Duplicate path detection**: Rejects recipes that list the same file path more than once

## Installation

```sh
cd tools/cmd/archive-content
go build -o ../../../build/tools/archive-content
```

## Usage

```sh
archive-content [options] <recipe.toml> [recipe2.toml ...]
```

One or more TOML recipe files are passed as positional arguments. Each recipe contains `[[group]]` entries that list the relative file paths to archive (see `examples/*/assets/archive/archive.toml`).

### Options

| Option | Description |
|--------|-------------|
| `-o <path>` | Output index file (`.idx`) |
| `--outbin <path>` | Output binary archive file (`.pak`) |
| `--outdebug <path>` | Output debug index file (`.idx-debug`). Omit to skip debug output |
| `--contentdir <path>` | Root directory that contains the source asset files |
| `--platform <name>` | Target platform name (e.g. `windows`, `emscripten`). Used to filter `target_platforms` groups. Repeatable — when multiple platforms are specified, a group is included if it matches any of them |
| `--depfile <path>` | Output a GCC-style depfile listing all packed files (optional). Used with Ninja for incremental builds |

### Examples

Archive assets for the quickstart example (Windows):

```sh
./build/tools/archive-content \
    --platform windows \
    --contentdir ./build/quickstart/content \
    -o ./build/quickstart/shipping/windows/content.idx \
    --outbin ./build/quickstart/shipping/windows/content.pak \
    --outdebug ./build/quickstart/archivebuild/windows/content.idx-debug \
    ./assets/archive/archive_fonts.toml \
    ./assets/archive/archive_shaders.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

Archive assets for the quickstart example (Emscripten / web):

```sh
./build/tools/archive-content \
    --platform emscripten \
    --contentdir ./build/quickstart/content \
    -o ./build/quickstart/shipping/web/content.idx \
    --outbin ./build/quickstart/shipping/web/content.pak \
    --outdebug ./build/quickstart/archive/web/content.idx-debug \
    ./assets/archive/archive_fonts.toml \
    ./assets/archive/archive_shaders.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

## Output Files

| File | Description |
|------|-------------|
| `content.idx` | FlatBuffers binary index (schema: `schemas/archive.fbs`). Shipped with the application |
| `content.pak` | Concatenated raw asset data. Shipped with the application |
| `content.idx-debug` | FlatBuffers debug index (schema: `schemas/archive_debug.fbs`). Development only |

## Depfile Support

When `--depfile` is specified, `archive-content` writes a GCC-style dependency file listing every source file that was packed into the archive. This enables Ninja to track individual file changes for incremental builds:

```sh
./build/tools/archive-content \
    --contentdir ./build/quickstart/content \
    -o ./build/quickstart/shipping/linux/content.idx \
    --outbin ./build/quickstart/shipping/linux/content.pak \
    --depfile ./build/quickstart/archivebuild/linux.d \
    ./assets/archive/archive_fonts.toml
```

The generated depfile has the format:

```makefile
build/quickstart/shipping/linux/content.idx: build/quickstart/content/fonts/NotoEmoji-Medium.ttf build/quickstart/content/fonts/Roboto-Medium.ttf ...
```

When combined with `archive-ninja-gen`, which generates Ninja build rules with `depfile` and `deps = gcc`, Ninja can detect changes to any file inside the archive and trigger a rebuild only when necessary.

## Recipe Format

Recipe files use TOML with `[[group]]` arrays. Each group lists relative file paths and an optional `target_platforms` filter.

```toml
[[group]]
paths = [
    "fonts/NotoSans-BoldItalic.ttf",
    "sounds/pong1.wav",
]

[[group]]
target_platforms = ["windows", "linux", "macos"]
paths = [
    "shaders/glsl/simple_effect_ps.glsl",
    "shaders/glsl/simple_effect_vs.glsl",
]

[[group]]
target_platforms = ["emscripten"]
paths = [
    "shaders/webgl/simple_effect_ps.glsl",
    "shaders/webgl/simple_effect_vs.glsl",
]
```
