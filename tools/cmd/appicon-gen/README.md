# appicon-gen

A tool for generating application icon files (`*.ico`) from PNG images. It also supports resizing PNG images with various resampling filters.

## Background

Windows executables (`.exe`) require a `*.ico` file as an icon resource.
Icon images are typically prepared as high-resolution PNGs (e.g. 512×512 or 1024×1024),
and need to be scaled down and converted to ICO format before embedding.
This tool handles both resizing and conversion in a single step.

The ICO file is written in the PNG-in-ICO format supported since Windows Vista.
Per the ICO specification, each entry is limited to a maximum of 256×256 pixels;
input images larger than this are automatically downscaled.
When multiple sizes are specified via `--sizes`, all of them are written into a single ICO file,
which is the format recommended by Microsoft for Windows application icons.

## Prerequisites

- **Go**: Required to build and run this tool

## Usage

```sh
appicon-gen -in <input.png> -o <output> [options]
```

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `-in <path>` | *(required)* | Input PNG file |
| `-o <path>` | *(required)* | Output file path (`*.png` or `*.ico`) |
| `-format <fmt>` | `png` | Output format: `png` or `ico` |
| `-sizes <list>` | *(unset)* | Comma-separated list of square icon sizes for multi-size ICO output (e.g. `16,32,48,256`). When set with `--format ico`, takes precedence over `-w`/`-h` |
| `-w <n>` | Input width | Output width in pixels (single-image path) |
| `-h <n>` | Input height | Output height in pixels (single-image path) |
| `-func <name>` | `CatmullRom` | Resampling filter: `CatmullRom`, `BiLinear`, `ApproxBiLinear`, `NearestNeighbor` (single-image path) |

### Examples

Generate a multi-size `*.ico` file (recommended for Windows application icons):

```sh
go run ./ -in icon_512x512.png -o app.ico --format ico --sizes 16,32,48,256
go run ./ -in icon_512x512.png -o app.ico --format ico --sizes 16,20,24,32,40,48,64,128,256
```

Generate a single-size `*.ico` file:

```sh
go run ./ -in icon_512x512.png -o app.ico -w 256 -h 256 --format ico
```

Resize to a specific size with a specific resampling filter:

```sh
go run ./ -in icon_512x512.png -o icon_64x64.png -w 64 -h 64 --func NearestNeighbor
go run ./ -in icon_512x512.png -o icon_64x64.png -w 64 -h 64 --func BiLinear
go run ./ -in icon_512x512.png -o icon_64x64.png -w 64 -h 64 --func ApproxBiLinear
go run ./ -in icon_512x512.png -o icon_64x64.png -w 64 -h 64 --func CatmullRom
```

Generate a resized `*.png` file:

```sh
go run ./ -in icon_512x512.png -o icon_256x256.png -w 256 -h 256 --func CatmullRom --format png
```
