# piskel2atlas

A tool for generating texture atlases from [Piskel](https://github.com/piskelapp/piskel) pixel art files (`*.piskel`).
It packs multiple sprites into a single atlas PNG texture and outputs a FlatBuffers binary (`.tileset`) containing region metadata for use at runtime.

## Background

[Piskel](https://github.com/piskelapp/piskel) is an open-source pixel art editor that saves sprites as `*.piskel` files — JSON documents with Base64-encoded PNG frames embedded in each layer.

Game asset pipelines typically require sprites to be packed into texture atlases to reduce draw calls and GPU state changes.
This tool reads one or more TOML recipe files that describe how Piskel frames should be assembled, renders each tile by compositing its layers, and packs the results into an atlas image via a bin-packing algorithm.

In addition to the PNG texture, the tool writes a companion `.tileset` file — a FlatBuffers binary encoding a `TextureAtlas` table (see `schemas/texture_atlas.fbs`).
Each region is keyed by the XXH32 hash of its sprite name for compact, O(log n) lookup at runtime.

The tool also supports extracting a single tile as a standalone PNG (with optional nearest-neighbor upscaling), which is useful for generating application icons or other single-image assets from the same Piskel source.

## Prerequisites

- **Go**: Required to build and run this tool

## Usage

### Generate a texture atlas

```sh
piskel2atlas -o <output.tileset> --out-tex <output.png> [options] <recipe.toml>...
```

### Extract a single tile as PNG

```sh
piskel2atlas --out-tex <output.png> -single <tile-name> [-resize <scale>] <recipe.toml>...
```

### Options

| Option | Description |
|--------|-------------|
| `-o <path>` | Output `.tileset` file path (required; ignored when `-single` is used) |
| `--out-tex <path>` | Output PNG texture file path (required) |
| `-single <name>` | Extract only the named tile as a standalone PNG instead of building the full atlas |
| `-resize <scale>` | Scale factor for nearest-neighbor upscaling when using `-single` (e.g. `2`, `4`) |
| `-d <path>` | Write a GCC-style dependency file (`.d`) listing all input Piskel files |

### Output files

When building a full atlas (`-single` is not specified):

| File | Description |
|------|-------------|
| `-o` path | FlatBuffers binary (`TextureAtlas` table, `schemas/texture_atlas.fbs`) |
| `--out-tex` path | Packed atlas texture |

## Recipe file format

Recipe files are TOML documents. Multiple recipe files can be passed on the command line; their tile lists are merged and the largest values for `width`, `height`, `per_tile_spacing`, and `tile_border_size` are used.

```toml
width  = 512   # atlas texture width in pixels
height = 512   # atlas texture height in pixels

# per_tile_spacing: gap in pixels added around every packed tile.
# Prevents adjacent-pixel bleed from bilinear filtering.
per_tile_spacing = 1

# tile_border_size: width in pixels of an edge-repeating border rendered
# outside each tile. Together with per_tile_spacing this enables
# pseudo-clamp texture sampling on atlases.
tile_border_size = 0

# tight_packing: when true, transparent margins are cropped before packing
# and the offset is stored in x_offset/y_offset in the tileset binary.
tight_packing = true

# premultiplied_alpha: when true, the PNG output and tileset flag use
# premultiplied alpha.
premultiplied_alpha = false

[[tiles]]
name   = "player_idle"   # sprite name used to derive the runtime hash key
source = "player.piskel" # path relative to this recipe file
index  = 1               # 1-based animation frame index within the Piskel file

[[tiles]]
name   = "player_run"
source = "player.piskel"
index  = 2
layers = [0, 1]          # optional: only composite these layer indices (0-based)

[[tiles]]
name   = "enemy"
source = "enemy.piskel"
index  = 1
opacity_layer = 2        # optional: use this layer's green channel as an alpha mask
```

### `mask_rect`

An optional clipping rectangle in tile-local pixel coordinates. Pixels outside the rectangle are cleared to transparent.

```toml
[[tiles]]
name   = "icon"
source = "icons.piskel"
index  = 1
[tiles.mask_rect]
min_x = 0
min_y = 0
max_x = 16
max_y = 16
```

## Examples

Build a full atlas from a single recipe:

```sh
piskel2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    ./assets/sprites/tiles.toml
```

Build an atlas from multiple recipes (tile lists are merged):

```sh
piskel2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    ./assets/sprites/characters.toml \
    ./assets/sprites/environment.toml
```

Build with a dependency file for incremental Ninja builds:

```sh
piskel2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    -d ./build/assets/sprites.d \
    ./assets/sprites/tiles.toml
```

Extract a single tile at 4× scale (e.g. for an application icon source):

```sh
piskel2atlas \
    --out-tex ./build/assets/icon_large.png \
    -single player_idle \
    -resize 4 \
    ./assets/sprites/tiles.toml
```
