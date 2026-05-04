# libgdx-atlas-to-tileset

A tool that converts a [Spine](https://esotericsoftware.com/) `.atlas` + PNG pair into Pomdog's FlatBuffers `.tileset` binary format with a freshly packed PNG texture.

## Background

Spine exports sprite atlases using the [libGDX atlas format](https://esotericsoftware.com/spine-atlas-format), which originated in the libGDX texture packer. The atlas is a pair of files: a PNG texture and a human-readable `.atlas` text file listing sprite regions.

Pomdog's runtime uses a compact FlatBuffers binary (`.tileset`) instead of the text format for faster loading and smaller binary size.

Unlike a pure format converter, this tool also **repacks** the sprites:

1. Each sprite is extracted from the input PNG.
2. If the sprite was stored rotated 90° CCW in the original atlas (when `rotate: true`), it is rotated back 90° CW to restore the original orientation.
3. All sprites (now in their original orientation) are bin-packed into a new PNG texture written to `--out-tex`.
4. A FlatBuffers `.tileset` binary is written to `-o`.

The `.tileset` file uses XXH32 hashes (seed `20160723`) of sprite names as keys, and the region list is sorted for O(log n) binary-search lookup via `TextureAtlas::getRegionByKey()`.

## Prerequisites

- **Go**: Required to build and run this tool

## Usage

```sh
libgdx-atlas-to-tileset \
    --in-atlas  <input.atlas> \
    --in-tex    <input.png>   \
    -o          <output.tileset> \
    --out-tex   <output.png>     \
    [--out-width  N] \
    [--out-height N] \
    [--out-per-tile-spacing N] \
    [--out-tight-packing=false] \
    [--out-premultiplied-alpha]
```

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `--in-atlas <path>` | — | Input Spine `.atlas` text file (required) |
| `--in-tex <path>` | — | Input PNG texture file referenced by the atlas (required) |
| `-o <path>` | — | Output `.tileset` FlatBuffers binary file (required) |
| `--out-tex <path>` | — | Output PNG texture file with repacked sprites (required) |
| `--out-width N` | same as input | Output texture width in pixels |
| `--out-height N` | same as input | Output texture height in pixels |
| `--out-per-tile-spacing N` | `2` | Gap in pixels between packed sprites (prevents colour bleed from bilinear filtering) |
| `--out-tight-packing=false` | `true` | When true, transparent margins are cropped before packing |
| `--out-premultiplied-alpha` | `false` | When set, RGB channels are premultiplied by alpha in the output PNG |

### Output files

- `<-o path>`: FlatBuffers `.tileset` binary, loadable via `createTextureAtlas()`
- `<--out-tex path>`: PNG texture with all sprites repacked in their original (unrotated) orientation

The tool exits with a non-zero status and an error message if any sprite does not fit in the output dimensions.

## Examples

Convert an atlas in place (same output dimensions as input):

```sh
libgdx-atlas-to-tileset \
    --in-atlas  ./assets/skeletal2d/MaidChan/skeleton.atlas \
    --in-tex    ./assets/skeletal2d/MaidChan/skeleton.png   \
    -o          ./build/skeletal2d/MaidChan/skeleton.tileset \
    --out-tex   ./build/skeletal2d/MaidChan/skeleton.png
```

Convert with a larger output texture and premultiplied alpha:

```sh
libgdx-atlas-to-tileset \
    --in-atlas  ./assets/ghost/Ghost.atlas \
    --in-tex    ./assets/ghost/Ghost.png   \
    -o          ./build/ghost/Ghost.tileset \
    --out-tex   ./build/ghost/Ghost.png     \
    --out-width  512 \
    --out-height 512 \
    --out-premultiplied-alpha
```

Disable tight packing (keep full sprite frames including transparent edges):

```sh
libgdx-atlas-to-tileset \
    --in-atlas  ./assets/ui.atlas \
    --in-tex    ./assets/ui.png   \
    -o          ./build/ui.tileset \
    --out-tex   ./build/ui.png     \
    --out-tight-packing=false
```

## Atlas file format

The tool understands the libGDX/Spine `.atlas` format:

```
Ghost.png
size: 174,174
format: RGBA8888
filter: Linear,Linear
repeat: none
Ghost_LeftArm
  rotate: false
  xy: 155, 156
  size: 19, 18
  orig: 19, 18
  offset: 0, 0
  index: -1
Ghost_Body
  rotate: true
  xy: 0, 0
  size: 60, 80
  orig: 60, 80
  offset: 0, 0
  index: -1
```

See the [Spine atlas format documentation](https://esotericsoftware.com/spine-atlas-format) for the full specification.
