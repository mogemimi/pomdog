# png2atlas

A tool for packing multiple PNG images into a texture atlas and writing a Pomdog FlatBuffers `.tileset` binary.

## Background

Game asset pipelines often need to combine individual sprite PNGs into a single atlas texture to reduce GPU state changes and draw calls at runtime.
This tool reads a TOML recipe listing PNG files, packs them into an atlas via a bin-packing algorithm, and writes two output files:

- A PNG texture containing all the packed sprites.
- A companion `.tileset` file — a FlatBuffers binary encoding a `TextureAtlas` table (see `schemas/texture_atlas.fbs`).

Each region is keyed by the XXH32 hash (seed `20160723`) of its sprite name for compact, O(log n) lookup at runtime via `TextureAtlas::getRegionByKey()`.

Transparent margins can be automatically trimmed (`tight_packing = true`) to reduce atlas waste.
Offset data is stored in the `.tileset` so that the engine can reconstruct correct sprite placement at runtime.

## Prerequisites

- **Go**: Required to build and run this tool

## Usage

```sh
png2atlas -o <output.tileset> --out-tex <output.png> [-d <output.d>] <recipe.toml>...
```

### Options

| Option | Description |
|--------|-------------|
| `-o <path>` | Output `.tileset` file path (required) |
| `--out-tex <path>` | Output PNG texture file path (required) |
| `-d <path>` | Write a GCC-style dependency file (`.d`) listing all input PNG files |

### Output files

| File | Description |
|------|-------------|
| `-o` path | FlatBuffers binary (`TextureAtlas` table, `schemas/texture_atlas.fbs`) |
| `--out-tex` path | Packed atlas texture |

## Recipe file format

Recipe files are TOML documents. Multiple recipe files can be passed on the command line; their entry lists are merged and the largest values for `width`, `height`, and `per_tile_spacing` are used.

```toml
width  = 512   # atlas texture width in pixels
height = 512   # atlas texture height in pixels

# per_tile_spacing: gap in pixels added around each packed sprite.
# Prevents adjacent-pixel bleed from bilinear filtering.
per_tile_spacing = 2

# tight_packing: when true, transparent margins are cropped before packing.
# The crop offsets are stored in x_offset/y_offset in the .tileset binary
# so that the engine can reconstruct correct sprite placement at runtime.
# Default is true for compact atlas size.
tight_packing = true

# premultiplied_alpha: when true, the PNG output and tileset flag use
# premultiplied alpha.
premultiplied_alpha = false

[[entries]]
name   = "player_idle"    # sprite name used to derive the runtime hash key
source = "player_idle.png" # path relative to this recipe file

[[entries]]
name   = "enemy_walk"
source = "enemy_walk.png"
```

## Examples

Build an atlas from a single recipe:

```sh
png2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    ./assets/sprites/sprites.toml
```

Build an atlas from multiple recipes (entry lists are merged):

```sh
png2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    ./assets/sprites/characters.toml \
    ./assets/sprites/environment.toml
```

Build with a dependency file for incremental Ninja builds:

```sh
png2atlas \
    -o ./build/assets/sprites.tileset \
    --out-tex ./build/assets/sprites.png \
    -d ./build/assets/sprites.d \
    ./assets/sprites/sprites.toml
```

## Tight packing and offsets

When `tight_packing = true`, transparent pixels around each sprite are cropped before packing.
The crop offsets (`x_offset`, `y_offset`) are stored in the `.tileset` binary.
At runtime, use `TextureRegion::xOffset` and `TextureRegion::yOffset` to reconstruct the correct position of the sprite within its original frame.
