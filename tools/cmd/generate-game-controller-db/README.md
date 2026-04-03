# generate-game-controller-db

A tool that converts the [SDL_GameControllerDB](https://github.com/gabomdq/SDL_GameControllerDB) text file (`gamecontrollerdb.txt`) into a FlatBuffers binary (`.gcdb`) for the Pomdog engine's runtime gamepad mapping system.

## Background

Pomdog previously embedded an auto-generated C++ header containing the entire SDL_GameControllerDB as a compiled-in array. This made the binary large and required a full rebuild whenever the upstream database was updated.

`generate-game-controller-db` replaces that approach by producing a compact FlatBuffers binary that the engine loads at runtime from the asset archive. Each mapping entry is keyed by `xxHash-64("UUID:PLATFORM")` with seed `20160723`, and the output table is sorted by key so that the C++ runtime can perform `LookupByKey()` binary search in O(log n).

## How it works

1. Parse `gamecontrollerdb.txt` line by line, extracting UUID, name, platform, button/axis mapping fields.
2. Normalise platform strings (`"Windows"` → `"windows"`, `"Mac OS X"` → `"macos"`, etc.).
3. For each entry, compute `xxHash-64(uuid + ":" + platform)` as the lookup key.
4. Detect hash collisions across all entries (fatal error if found).
5. Build a `GameControllerDB` FlatBuffers table with mappings sorted by key.
6. Optionally build a `GameControllerDBDebug` table containing human-readable UUID, name, and platform strings for debugging.
7. Write the binary (`.gcdb`) and optional debug binary (`.gcdb-debug`) to disk.

## Schemas

The tool uses two FlatBuffers schemas:

- `schemas/game_controller_db.fbs` — runtime binary (sorted table with `LookupByKey`)
- `schemas/game_controller_db_debug.fbs` — debug companion with readable strings

## Installation

```sh
cd tools/cmd/generate-game-controller-db
go build -o ../../../build/tools/generate-game-controller-db
```

## Usage

```sh
generate-game-controller-db -i <input> -o <output> [-outdebug <debug-output>]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input SDL_GameControllerDB file (`gamecontrollerdb.txt`) |
| `-o <path>` | Output game controller DB file (`*.gcdb`) |
| `-outdebug <path>` | Output debug file (`*.gcdb-debug`, optional) |

### Examples

Convert the vendored game controller database:

```sh
./build/tools/generate-game-controller-db \
    -i ./thirdparty/SDL_GameControllerDB/gamecontrollerdb.txt \
    -o ./build/convertbuild/game_controller_db/gamecontrollerdb.gcdb \
    -outdebug ./build/convertbuild/game_controller_db/gamecontrollerdb.gcdb-debug
```

## Build pipeline integration

`asset-convert-ninja-gen` automatically generates a Ninja build rule for this tool. The pipeline becomes:

```
gamecontrollerdb.txt → generate-game-controller-db → .gcdb → archive
```

The recipe is defined in `assets/assetconvert.toml`:

```toml
[[game_controller_db]]
use_abs_path_indir = true
in = "${POMDOG_THIRDPARTY_DIR}/SDL_GameControllerDB/gamecontrollerdb.txt"
out = "game_controller_db/gamecontrollerdb.gcdb"
```

## Mapping details

### Supported SDL fields

Buttons: `a`, `b`, `x`, `y`, `guide`, `back`, `start`, `leftshoulder`, `rightshoulder`, `leftstick`, `rightstick`, `dpup`, `dpdown`, `dpleft`, `dpright`, `lefttrigger`, `righttrigger`.

Axes: `leftx`, `lefty`, `rightx`, `righty`.

Hats: `h0.1` (up), `h0.2` (right), `h0.4` (down), `h0.8` (left). Only hat 0 is supported. Hat directions are stored in a 4-element `hats` array inside the FlatBuffers binary, and the platform backends use them to route hat/POV inputs through the mapping table.

### Platforms

`Windows`, `Linux`, `Mac OS X`, `Android`, `iOS`. Entries with unrecognised platform strings are skipped.
