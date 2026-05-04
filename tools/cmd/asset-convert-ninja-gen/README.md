# asset-convert-ninja-gen

A Ninja build file generator for Pomdog's asset conversion pipeline. It reads a TOML recipe describing which assets to convert and produces a `build.ninja` file that drives the actual conversion using dedicated tools.

## Background

Pomdog's asset pipeline converts source data (text databases, raw audio, sprite sheets, etc.) into optimised binary formats suitable for the runtime. `asset-convert-ninja-gen` is the orchestrator: it does not perform conversions itself, but generates Ninja build rules that invoke the appropriate tools.

This design mirrors the existing `shader-ninja-gen` tool that generates Ninja rules for shader compilation, keeping the build system consistent and incremental.

## How it works

1. Read a TOML recipe file (e.g. `assets/assetconvert.toml`).
2. Substitute environment variables (`${POMDOG_THIRDPARTY_DIR}`) with the value of `-thirdpartydir`.
3. Validate the recipe.
4. For each conversion table array, emit Ninja rules and build edges.
5. Create any required output and intermediate directories.
6. Write the resulting `build.ninja` file.

## Installation

```sh
cd tools/cmd/asset-convert-ninja-gen
go build -o ../../../build/tools/asset-convert-ninja-gen
```

## Usage

```sh
asset-convert-ninja-gen [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-recipe <path>` | Input TOML recipe file (required) |
| `-indir <path>` | Base input directory for relative paths |
| `-thirdpartydir <path>` | Third-party directory (replaces `${POMDOG_THIRDPARTY_DIR}` in the recipe) |
| `-outninja <path>` | Output Ninja file path (required) |
| `-outdir <path>` | Output content directory (required) |
| `-intdir <path>` | Intermediate directory |
| `-tooldir <path>` | Directory containing conversion tools (required) |
| `-verbose` | Show verbose ffmpeg output |

### Example

```sh
./build/tools/asset-convert-ninja-gen \
    -recipe ./assets/assetconvert.toml \
    -indir ./assets \
    -thirdpartydir ./thirdparty \
    -intdir ./build/convertbuild \
    -outninja ./build/convertbuild/convert_pomdog.ninja \
    -outdir ./build/content \
    -tooldir ./build/tools
```

Then run the generated Ninja file:

```sh
./build/tools/ninja -f ./build/convertbuild/convert_pomdog.ninja
```

## Recipe format

The recipe is a TOML file with one or more conversion table arrays.

---

### `[[game_controller_db]]`

Converts an [SDL_GameControllerDB](https://github.com/mdqinc/SDL_GameControllerDB) text file to a `.gcdb` binary.

| Field | Type | Description |
|-------|------|-------------|
| `in` | string | Input file path (relative to `-indir`, or absolute when `use_abs_path_indir` is true) |
| `out` | string | Output file path (relative to `-outdir`) |
| `use_abs_path_indir` | bool | If true, treat `in` as an absolute path instead of relative to `-indir` |

---

### `[[sound_clip]]`

Converts a WAV file to WAV or Ogg Vorbis, with optional crossfade loop processing.

| Field | Type | Description |
|-------|------|-------------|
| `in_file` | string | Input file path (relative to `-indir`) |
| `out_file` | string | Output file path (relative to `-outdir`) |
| `in_format` | string | Input format: `"wav"` |
| `out_format` | string | Output format: `"wav"` or `"vorbis"` |
| `wav_options` | table | WAV output options (`channels`, `sample_rate`, `bit_depth`) |
| `vorbis_options` | table | Vorbis output options (`channels`, `sample_rate`, `bit_rate`) |
| `loop` | table | Crossfade loop parameters (optional) |

#### `loop` table

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `trim_start` | bool | `false` | Remove the crossfade region from the start |
| `seconds` | float | `0` | Crossfade duration in seconds |
| `align_samples` | int | `0` | Align loop sample count to this boundary |
| `fade_method` | string | `"equal_power"` | Crossfade curve: `"linear"`, `"equal_power"`, or `"logarithmic"` |

---

### `[[tileset_from_pngs]]`

Packs PNG sprite images described by a `png2atlas` recipe TOML into a texture atlas PNG and `.tileset` binary. Invokes `png2atlas`. Supports GCC-style dependency tracking for incremental rebuilds.

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `recipe` | string | ✓ | `png2atlas` recipe TOML file (relative to `-indir`) |
| `out_tileset` | string | ✓ | Output `.tileset` binary (relative to `-outdir`) |
| `out_texture` | string | ✓ | Output PNG texture (relative to `-outdir`) |

---

### `[[tileset_from_piskel]]`

Renders [Piskel](https://github.com/piskelapp/piskel) sprites described by one or more `piskel2atlas` recipe TOMLs into a texture atlas PNG and `.tileset` binary. Invokes `piskel2atlas`. Supports GCC-style dependency tracking for incremental rebuilds.

Multiple recipe files can be specified in `in_recipe`; their tile lists are merged by `piskel2atlas`.

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `in_recipe` | []string | ✓ | One or more `piskel2atlas` recipe TOML files (relative to `-indir`) |
| `out_tileset` | string | ✓ | Output `.tileset` binary (relative to `-outdir`) |
| `out_texture` | string | ✓ | Output PNG texture (relative to `-outdir`) |

---

### `[[tileset_from_spine_atlas]]`

Extracts sprites from a [Spine/libGDX `.atlas`](https://esotericsoftware.com/spine-atlas-format) + PNG pair, un-rotates any `rotate: true` regions, repacks them into a new PNG, and writes a FlatBuffers `.tileset` binary. Invokes `libgdx-atlas-to-tileset`.

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `in_atlas` | string | ✓ | Input `.atlas` text file (relative to `-indir`) |
| `in_image` | string | ✓ | Input PNG texture referenced by the atlas (relative to `-indir`) |
| `out_tileset` | string | ✓ | Output `.tileset` binary (relative to `-outdir`) |
| `out_texture` | string | ✓ | Output PNG texture (relative to `-outdir`) |
| `width` | int | | Output texture width in pixels (default: same as input) |
| `height` | int | | Output texture height in pixels (default: same as input) |
| `per_tile_spacing` | int | | Gap in pixels between sprites (default: 2) |
| `tight_packing` | bool | | Crop transparent margins before packing (default: true) |
| `premultiplied_alpha` | bool | | Premultiply alpha in output texture (default: false) |

---

## Example recipe

```toml
[[game_controller_db]]
use_abs_path_indir = true
in = "${POMDOG_THIRDPARTY_DIR}/SDL_GameControllerDB/gamecontrollerdb.txt"
out = "game_controller_db/gamecontrollerdb.gcdb"

# Convert WAV to WAV (resample, no loop)
[[sound_clip]]
in_file = "sounds/ambient_lakeside.wav"
out_file = "sounds/ambient_lakeside_oneshot.wav"
in_format = "wav"
out_format = "wav"
wav_options = {channels = 2, sample_rate = 22050, bit_depth = 16}

# Convert WAV to WAV with crossfade loop
[[sound_clip]]
in_file = "sounds/ambient_lakeside.wav"
out_file = "sounds/ambient_lakeside_wav.wav"
in_format = "wav"
out_format = "wav"
wav_options = {channels = 1, sample_rate = 44100, bit_depth = 16}
loop = {trim_start = true, seconds = 2.0, align_samples = 4096}

# Convert WAV to Ogg Vorbis with crossfade loop
[[sound_clip]]
in_file = "sounds/ambient_lakeside.wav"
out_file = "sounds/ambient_lakeside_vorbis.ogg"
in_format = "wav"
out_format = "vorbis"
vorbis_options = {channels = 1, sample_rate = 44100, bit_rate = 128}
loop = {trim_start = true, seconds = 2.0, align_samples = 4096}

# Pack a set of PNG sprites into a tileset using a png2atlas recipe
[[tileset_from_pngs]]
recipe = "sprites/ui.toml"
out_tileset = "sprites/ui.tileset"
out_texture = "sprites/ui.png"

# Pack Piskel animation frames into a tileset (multiple recipes merged)
[[tileset_from_piskel]]
out_tileset = "effects/particles.tileset"
out_texture = "effects/particles.png"
in_recipe = [
    "effects/particles_fire.toml",
    "effects/particles_water.toml",
]

# Repack a Spine atlas to a Pomdog tileset (with rotation fix and premultiplied alpha)
[[tileset_from_spine_atlas]]
in_atlas = "skeletal2d/MaidGun/MaidGun.atlas"
in_image = "skeletal2d/MaidGun/MaidGun.png"
out_tileset = "skeletal2d/MaidGun/MaidGun.tileset"
out_texture = "skeletal2d/MaidGun/MaidGun.png"
width = 512
height = 512
per_tile_spacing = 2
tight_packing = true
premultiplied_alpha = true
```

## Build pipeline integration

`assetbuild.sh` invokes `asset-convert-ninja-gen` as part of the asset build pipeline. The generated Ninja file is included via `subninja` in the main build:

```
TOML recipe → asset-convert-ninja-gen → build.ninja → ninja → converted assets → archive
```

## Extending

To add a new asset type, add a new TOML table array (e.g. `[[font_atlas]]`) to the recipe format:

1. Add the struct and slice field to `ConvertRecipe` in `main.go`.
2. Add a Ninja rule variable, rule, and build edges in a new `build<Type>()` function.
3. Add validation in `validateRecipe()`.
4. Call the new function from `run()`.
