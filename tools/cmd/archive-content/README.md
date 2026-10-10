# archive-content

An offline tool that packs multiple asset files into a single binary archive (`.pak`) with an accompanying FlatBuffers index (`.idx`). At runtime the engine maps file path hashes to byte ranges inside the `.pak`, enabling efficient O(log n) asset lookup without filesystem directory traversal.

The index stores only xxHash-64 hashes of file paths to minimise shipping binary size. A separate debug index (`.idx-debug`) retains the original path strings, the hash seed, a hash of each file's content, and the compression method requested for each file for diagnostic use; it is not included in release builds.

## Features

- **Single-file archive**: Stores all asset files in one `.pak` binary for fast sequential I/O
- **Per-file compression**: Stores each file as-is or as one zstd frame, as the archive settings describe for the platform, and stores a file as-is when compression saves too little
- **zstd dictionaries**: Stores the dictionaries that the zstd frames of small files use in the index
- **Shared data**: Stores identical bytes once, and lets keys of files stored in the same way share one entry
- **FlatBuffers index**: Generates a compact `.idx` with sorted hash keys for `LookupByKey()` binary search
- **Debug index**: Emits an `.idx-debug` with human-readable paths, content hashes, requested compression methods and dictionaries, and the xxHash seed for hash verification
- **Platform filtering**: Supports `target_platforms` in the TOML recipe to include/exclude assets per platform
- **Hash collision detection**: Aborts with an error if two different file paths produce the same xxHash-64 value
- **Duplicate path detection**: Rejects recipes that list the same file path more than once

`archive-content` does not compress files itself. It packs the zstd frames and dictionaries that the zstd command-line tool builds, usually through the Ninja build that [archive-ninja-gen](../archive-ninja-gen/README.md) generates.

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
| `--settings <path>` | Archive settings file (`.toml`). **Required**. See [Archive Settings](#archive-settings) |
| `--platform <name>` | Target platform name (`windows`, `macos`, `linux`, or `web`). **Required**. Selects the strategy in the archive settings and filters `target_platforms` groups. Repeatable — a group is included if it matches any of the platforms, and all of them must use the same strategy |
| `--zstddir <path>` | Directory of the zstd frames and dictionaries. Required when a file requests zstd. See [zstd Frames and Dictionaries](#zstd-frames-and-dictionaries) |

### Examples

Archive assets for the quickstart example (Windows):

```sh
./build/tools/archive-content \
    --platform windows \
    --settings ./assets/archive/archive_settings.toml \
    --contentdir ./build/quickstart/content \
    -o ./build/quickstart/shipping/windows/content.idx \
    --outbin ./build/quickstart/shipping/windows/content.pak \
    --outdebug ./build/quickstart/archivebuild/windows/content.idx-debug \
    ./assets/archive/archive_fonts.toml \
    ./build/quickstart/archive/build/archive_shaders_pomdog.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

Archive assets for the quickstart example (Emscripten / web), after the asset build has compressed the files:

```sh
./build/tools/archive-content \
    --platform web \
    --settings ./assets/archive/archive_settings.toml \
    --contentdir ./build/quickstart/content \
    --zstddir ./build/quickstart/archivebuild/web/zstd \
    -o ./build/quickstart/shipping/web/content.idx \
    --outbin ./build/quickstart/shipping/web/content.pak \
    --outdebug ./build/quickstart/archivebuild/web/content.idx-debug \
    ./assets/archive/archive_fonts.toml \
    ./build/quickstart/archive/build/archive_shaders_pomdog.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

## Output Files

| File | Description |
|------|-------------|
| `content.idx` | FlatBuffers binary index (schema: `schemas/archive.fbs`) with the zstd dictionaries. Shipped with the application |
| `content.pak` | Stored file data, each file stored as-is or as one zstd frame. Shipped with the application |
| `content.idx-debug` | FlatBuffers debug index (schema: `schemas/archive_debug.fbs`). Development only |

## Archive Settings

The settings file describes a strategy for the archive of each platform. `archive-content` uses the strategy whose `target_platforms` include the `--platform` values. Pomdog's example asset build uses `assets/archive/archive_settings.toml`; a game can pass its own settings file instead.

```toml
[[strategy]]
target_platforms = ["windows", "macos", "linux"]
compression = "none"

[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 32
max_ratio_percent = 95

[[strategy.zstd.dictionary]]
name = "structured"
max_size = 4096
paths = [
    "**/*.json",
    "shaders/webgl/*.glsl",
]
```

| Key | Description |
|-----|-------------|
| `target_platforms` | Platforms whose archives use the strategy: `windows`, `macos`, `linux`, or `web`. A platform belongs to at most one strategy |
| `compression` | Method for recipe groups that omit `compression`: `none` stores the file bytes as-is, and `zstd` stores one Zstandard frame with the content size and an XXH64 checksum |
| `zstd.level` | zstd compression level, from 1 to 22 |
| `zstd.min_saved_bytes` | Minimum number of bytes that a zstd frame must save (at least 1) |
| `zstd.max_ratio_percent` | Maximum size of a zstd frame as a percentage of the file size (greater than 0 and at most 100) |
| `zstd.dictionary.name` | Name of the dictionary in the debug index and the tool output. Letters, digits, `_`, and `-` |
| `zstd.dictionary.max_size` | Maximum size of the dictionary in bytes (at least 256) |
| `zstd.dictionary.paths` | Glob patterns of the files that use the dictionary, as listed in the recipes. `*`, `?`, and `[...]` match within a path segment, and a `**` segment matches any number of segments |

The `[strategy.zstd]` table and its `level`, `min_saved_bytes`, and `max_ratio_percent` keys are required when the strategy or a recipe group in its archives selects zstd. A file that requests zstd is stored as a frame only if the frame satisfies both conditions; otherwise the file is stored as-is, because decoding would cost more than the saved bytes are worth. Unknown keys and platform names are errors.

Each `[[strategy.zstd.dictionary]]` is trained on the files that request zstd and match its `paths`, and those files are compressed with it. A file that matches two dictionaries is an error. Dictionaries are optional, and a strategy can have at most 256 of them, because an index entry refers to its dictionary with an 8-bit index. The index stores only the dictionaries that its entries use. The dictionary ID of a dictionary is its position in the strategy plus one.

The debug index records the method and dictionary that each file requested and the size that they produced, so `archive-inspect -debug` lists the files stored as-is after a fallback.

## zstd Frames and Dictionaries

For each file that requests zstd, `archive-content` reads a frame of the file from `--zstddir`, and reads the dictionaries that those frames use:

| Path | Content |
|------|---------|
| `<zstddir>/files/<path>.zst` | One zstd frame of the file at `<path>` in the recipes |
| `<zstddir>/dictionaries/<id>.dict` | The dictionary with the dictionary ID `<id>` in the Zstandard dictionary format |

A missing frame or dictionary is an error that names its path. `archive-content` also checks that each frame records the size of its file and the ID of its dictionary, and that each dictionary has the expected ID, so stale frames do not reach the archive. [archive-ninja-gen](../archive-ninja-gen/README.md) generates the Ninja build that writes these files with the zstd command-line tool.

## Shared Data

Files whose stored bytes are identical share them in the `.pak` file, even if they restore different files, such as an uncompressed file whose content equals another file's frame. Keys of files stored in the same way share one index entry. The index does not describe the order of the stored data in the `.pak` file.

The output is deterministic: the same inputs, recipes, settings, frames, and dictionaries produce byte-identical `.idx`, `.pak`, and `.idx-debug` files.

## Recipe Format

Recipe files use TOML with `[[group]]` arrays. Each group lists relative file paths, an optional `target_platforms` filter, and an optional `compression` method that overrides the strategy's method for the group's files. Unknown keys and platform names are errors, so a misspelled key or platform name does not silently change how an archive stores its files or which files it holds.

```toml
[[group]]
paths = [
    "fonts/NotoSans-BoldItalic.ttf",
    "sounds/pong1.wav",
]

[[group]]
compression = "none"
paths = [
    "textures/pomdog.png",
]

[[group]]
target_platforms = ["windows", "linux", "macos"]
paths = [
    "shaders/glsl/simple_effect_ps.glsl",
    "shaders/glsl/simple_effect_vs.glsl",
]

[[group]]
target_platforms = ["web"]
compression = "zstd"
paths = [
    "shaders/webgl/simple_effect_ps.glsl",
    "shaders/webgl/simple_effect_vs.glsl",
]
```
