# asset-convert-ninja-gen

A Ninja build file generator for Pomdog's asset conversion pipeline. It reads a TOML recipe describing which assets to convert and produces a `build.ninja` file that drives the actual conversion using dedicated tools.

## Background

Pomdog's asset pipeline converts source data (text databases, raw audio, etc.) into optimised binary formats suitable for the runtime. `asset-convert-ninja-gen` is the orchestrator: it does not perform conversions itself, but generates Ninja build rules that invoke the appropriate tools (e.g. `generate-game-controller-db`, `wav-crossfade-conv`, `ffmpeg`).

This design mirrors the existing `shader-ninja-gen` tool that generates Ninja rules for shader compilation, keeping the build system consistent and incremental.

## How it works

1. Read a TOML recipe file (e.g. `assets/assetconvert.toml`).
2. Substitute environment variables (`${POMDOG_THIRDPARTY_DIR}`) with the value of `-thirdpartydir`.
3. Validate the recipe.
4. For each `[[game_controller_db]]` entry, emit a Ninja build edge calling `generate-game-controller-db`.
5. For each `[[sound_clip]]` entry, emit Ninja build edges for crossfade (optional) and format conversion via `ffmpeg`.
6. Create any required output and intermediate directories.
7. Write the resulting `build.ninja` file.

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

### `[[game_controller_db]]`

| Field | Type | Description |
|-------|------|-------------|
| `in` | string | Input file path (relative to `-indir`, or absolute when `use_abs_path_indir` is true) |
| `out` | string | Output file path (relative to `-outdir`) |
| `use_abs_path_indir` | bool | If true, treat `in` as an absolute path instead of relative to `-indir` |

### `[[sound_clip]]`

Each `[[sound_clip]]` entry defines a single audio conversion.

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

### Example recipe

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
```

## Build pipeline integration

`assetbuild.sh` invokes `asset-convert-ninja-gen` as part of the asset build pipeline. The generated Ninja file is included via `subninja` in the main build:

```
TOML recipe → asset-convert-ninja-gen → build.ninja → ninja → converted assets → archive
```

## Extending

To add a new asset type, add a new TOML table array (e.g. `[[font_atlas]]`) to the recipe format:

1. Add the struct and slice field to `ConvertRecipe` in `main.go`.
2. Add a Ninja rule and build edges for the new type.
3. Add validation in `validateRecipe()`.
