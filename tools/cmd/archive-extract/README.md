# archive-extract

A diagnostic tool that restores the original files from an archive (`.idx` + `.pak`) produced by `archive-content`. It decompresses zstd entries with the zstd command-line tool, including entries compressed with the dictionaries in the index, so you can examine shipped content offline, compare it with the source assets, or check what a player's archive contains.

## Installation

```sh
cd tools/cmd/archive-extract
go build -o ../../../build/tools/archive-extract
```

## Usage

```sh
archive-extract [options] [path ...]
```

Positional arguments are file paths as listed in the archive recipes, such as `fonts/Roboto-Medium.ttf`. The tool looks them up by the same xxHash-64 keys that the engine uses. Without positional arguments, it extracts every entry.

### Options

| Option | Description |
|--------|-------------|
| `-idx <path>` | Input index file (`.idx`). **Required** |
| `-pak <path>` | Input binary archive file (`.pak`). **Required** |
| `-debug <path>` | Input debug index file (`.idx-debug`). Restores paths when extracting every entry and verifies each restored file against its content hash |
| `-o <dir>` | Output directory. **Required**. Files are written to `<dir>/<path>`, overwriting existing files |
| `-zstd <path>` | zstd command-line tool. Required to extract zstd entries. A path or a command name in `PATH`, such as `./build/tools/zstd` |

Without `-debug`, a shipping index has no paths, so extracting every entry names each file by its key in hexadecimal (e.g. `<dir>/8c150e68e1f45d6c`). `archive-inspect` lists the keys with their sizes and compression methods.

The tool stops with an error if an entry lies outside the `.pak` file, does not decode to the size in the index, or does not match its content hash in the debug index. It also rejects paths that would be written outside the output directory.

### Examples

Restore every file of the web archive with its original path:

```sh
./build/tools/archive-extract \
    -idx ./build/quickstart/shipping/web/content.idx \
    -pak ./build/quickstart/shipping/web/content.pak \
    -debug ./build/quickstart/archivebuild/web/content.idx-debug \
    -zstd ./build/tools/zstd \
    -o ./build/quickstart/extracted/web
```

Restore specific files from a shipping archive without debug info:

```sh
./build/tools/archive-extract \
    -idx ./build/quickstart/shipping/web/content.idx \
    -pak ./build/quickstart/shipping/web/content.pak \
    -zstd ./build/tools/zstd \
    -o ./build/quickstart/extracted/web \
    fonts/Roboto-Medium.ttf \
    shaders/webgl/sprite_batch_ps.glsl
```
