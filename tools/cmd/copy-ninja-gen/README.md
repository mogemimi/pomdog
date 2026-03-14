# copy-ninja-gen

A code generator that produces [Ninja](https://ninja-build.org/) build files for asset file copy operations. It reads a `copybuild.toml` recipe and generates build rules that copy source asset files to the content output directory.

In the Pomdog asset pipeline, many non-shader assets (fonts, textures, sounds, SVG icons, skeletal animation data, etc.) need to be copied from their source locations into a flat content directory before being packed into the shipping archive. Previously, this was done with ad-hoc `cp` commands in `assetbuild.sh`, which ran unconditionally every time. By expressing these copies as Ninja build edges, only files whose source has actually changed are re-copied on subsequent builds, significantly reducing incremental build times for large projects like `feature_showcase`.

## Features

- **TOML recipe driven**: Declarative `copybuild.toml` replaces hand-written `cp` commands in shell scripts
- **Single file mapping (`[[file]]`)**: Explicit 1:1 source-to-destination path mapping
- **Batch mapping (`[[file_group]]`)**: Regex-based path transformation for copying many files with a single rule
- **Regex validation**: Errors if any `src` path in a `[[file_group]]` does not match the `regex`, catching typos early
- **Incremental builds**: Ninja tracks file timestamps so unchanged files are not re-copied
- **Deterministic output**: Build edges are sorted by destination path for reproducible Ninja files

## Installation

```sh
cd tools/cmd/copy-ninja-gen
go build -o ../../../build/tools/copy-ninja-gen
```

## Usage

```sh
copy-ninja-gen [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-recipe <path>` | Path to the `copybuild.toml` recipe file |
| `-indir <path>` | Root directory containing the source asset files |
| `-outninja <path>` | Output Ninja build file path |
| `-outdir <path>` | Output content directory where files are copied to |

### Examples

Generate Ninja build file for engine fonts:

```sh
./build/tools/copy-ninja-gen \
    -recipe ./assets/copybuild.toml \
    -indir ./assets \
    -outninja ./build/quickstart/copybuild/copy_pomdog.ninja \
    -outdir ./build/quickstart/content
```

Generate Ninja build file for application-specific assets:

```sh
./build/tools/copy-ninja-gen \
    -recipe ./examples/feature_showcase/assets/copybuild.toml \
    -indir ./examples/feature_showcase/assets \
    -outninja ./build/feature_showcase/copybuild/copy_app.ninja \
    -outdir ./build/feature_showcase/content
```

## Configuration

The tool reads TOML recipe files (`copybuild.toml`) that define file copy mappings.

### `[[file]]` — Single file copy

| Field | Required | Description |
|-------|----------|-------------|
| `src` | Yes | Source file path relative to `-indir` |
| `dst` | Yes | Destination file path relative to `-outdir` |

### `[[file_group]]` — Batch copy with regex

| Field | Required | Description |
|-------|----------|-------------|
| `regex` | Yes | Regular expression matched against each `src` path |
| `dst` | Yes | Destination path template with `$1`, `$2` capture group references |
| `src` | Yes | Array of source file paths relative to `-indir` |

Every `src` entry must match the `regex`; a mismatch is treated as an error to catch typos in the recipe.

Example `copybuild.toml`:

```toml
[[file]]
src = "fonts/NotoEmoji/NotoEmoji-Medium.ttf"
dst = "fonts/NotoEmoji-Medium.ttf"

[[file_group]]
regex = "svg/icons/([\w\-]+\.svg)"
dst = "svg/$1"
src = [
    "svg/icons/arrow-back.svg",
    "svg/icons/arrow-down.svg",
    "svg/icons/arrow-forward.svg",
]
```
