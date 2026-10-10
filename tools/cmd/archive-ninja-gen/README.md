# archive-ninja-gen

A code generator that produces [Ninja](https://ninja-build.org/) build files for the asset archive packaging step. It generates build rules that compress files with the zstd command-line tool and invoke `archive-content` to pack all processed asset files into shipping archive files (`content.idx` + `content.pak`).

In the Pomdog asset pipeline, the final step is to pack all content files — compiled shaders, fonts, textures, sounds, etc. — into a single binary archive for each target platform. Wrapping this step in Ninja build rules lets the asset build compress files in parallel and rebuild an archive only when one of its inputs changes.

## Features

- **Per-platform output**: Generates build rules for each platform (windows, macos, linux, web) in a single Ninja file
- **Archive settings**: Reads one archive settings file and passes it to every platform archive, which takes the strategy for its platform
- **Per-file compression**: Compresses each file that requests zstd in its own build edge, so Ninja runs the zstd command-line tool in parallel and reruns it only for changed files
- **Dictionary training**: Trains each zstd dictionary in the archive settings on the files that use it
- **Explicit dependencies**: Each archive depends on every file that `archive-content` reads, so Ninja builds the content files first and rebuilds the archive when one changes
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
| `-intdir <path>` | Intermediate directory for debug output, zstd frames, and dictionaries |
| `-tooldir <path>` | Directory containing the `archive-content`, `zstd`, and `zstd-wrapper` executables |
| `-settings <path>` | Archive settings file (`.toml`) passed to `archive-content`. **Required**. Every generated archive's platform needs a strategy in it |

Positional arguments are paths to `archive*.toml` recipe files.

### Archive Settings and Recipes

The tool reads the settings and recipes when it generates the Ninja file, so a platform without a strategy or a malformed recipe fails before the build runs. All recipes must exist at that time; the asset build generates the shader recipes with `shader-archive-gen` before it runs this tool. The generated file lists the files of the recipes, so regenerate it after editing the settings or recipes.

Pomdog's example asset build passes `assets/archive/archive_settings.toml`. See [archive-content](../archive-content/README.md#archive-settings) for the settings format and [Asset Pipeline and Runtime](../../../docs/asset-pipeline-and-runtime.md#compression) for the compression design and its runtime support.

### Build Edges

For each platform archive, the generated Ninja file contains:

- A `zstd_train` edge for each dictionary that a file uses. It trains the dictionary with one thread and the fixed dictionary ID from the settings, so the dictionary depends only on its files and settings. The tool writes the list of training files next to the dictionary and rewrites it only when it changes.
- A `zstd_compress` or `zstd_compress_dictionary` edge for each file that requests zstd, at the level in the settings.
- An `archive_content` edge that depends on the recipes, the settings, the content files, the zstd frames, and the dictionaries.

The zstd edges run `zstd` through [zstd-wrapper](../zstd-wrapper/README.md), because zstd gives each frame the modification time of its file, which would make the frame look older than its inputs. The wrapper gives each output the time when zstd wrote it.

Each edge also depends on the executable that produces its data: `zstd` for the zstd edges, and `archive-content` for the archives. Updating a tool in `-tooldir` rebuilds the data that it produces, and the archives that contain that data. `zstd-wrapper` only sets times, so no edge depends on it. Bootstrap rebuilds the Go tools on every run, so the next asset build packs every archive again.

### Examples

Generate Ninja build file for the quickstart example:

```sh
./build/tools/archive-ninja-gen \
    -outninja ./build/quickstart/archivebuild/build.ninja \
    -contentdir ./build/quickstart/content \
    -outdir ./build/quickstart/shipping \
    -intdir ./build/quickstart/archivebuild \
    -tooldir ./build/tools \
    -settings ./assets/archive/archive_settings.toml \
    ./assets/archive/archive_fonts.toml \
    ./build/quickstart/archive/build/archive_shaders_pomdog.toml \
    ./build/quickstart/archive/build/archive_shaders_quickstart.toml \
    ./examples/quickstart/assets/archive/archive.toml
```

The generated file depends on content files that other Ninja files build, so include them in one build, as `subninja-gen` does in the asset build.

## Output Directory Structure

```
<outdir>/
├── windows/
│   ├── content.idx           # FlatBuffers index for Windows
│   └── content.pak           # Binary archive for Windows
├── macos/
│   ├── content.idx           # FlatBuffers index for macOS
│   └── content.pak           # Binary archive for macOS
├── linux/
│   ├── content.idx           # FlatBuffers index for Linux
│   └── content.pak           # Binary archive for Linux
└── web/
    ├── content.idx           # FlatBuffers index for Emscripten / WebGL
    └── content.pak           # Binary archive for Emscripten / WebGL

<intdir>/
├── windows/
│   └── content.idx-debug     # Debug index with human-readable paths (development only)
├── macos/
│   └── content.idx-debug     # Debug index for macOS (development only)
├── linux/
│   └── content.idx-debug     # Debug index for Linux (development only)
└── web/
    ├── content.idx-debug     # Debug index for web platform (development only)
    └── zstd/
        ├── files/            # A zstd frame for each file that requests zstd
        └── dictionaries/     # Trained dictionaries and the lists of their training files
```
