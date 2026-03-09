# spirv-shader-reflect

A tool that extracts reflection data from SPIR-V shader modules and serializes it to FlatBuffers binary format. The output `.reflect` files are loaded by the pomdog engine at runtime to bind uniform buffers and samplers to the correct slots.

## Features

- **UBO reflection**: Extracts uniform buffer object names and binding indices
- **Sampler reflection**: Extracts sampler/texture names and binding indices
- **FlatBuffers output**: Serializes reflection data to compact binary format for efficient runtime loading
- **Debug output**: Optionally generates a separate debug reflection file with human-readable string names (not included in shipping builds)
- **Sorted output**: Sorts entries by name for binary search lookup at runtime

## Prerequisites

- **spirv-cross**: Used to extract reflection JSON from SPIR-V modules

## Installation

```sh
cd tools/cmd/spirv-shader-reflect
go build -o ../../../build/tools/spirv-shader-reflect
```

## Usage

```sh
spirv-shader-reflect [options]
```

### Options

| Option | Required | Description |
|--------|----------|-------------|
| `-i <path>` | Yes | Path to input SPIR-V file (`*.spv`) |
| `-o <path>` | Yes | Path to output reflection file (`*.reflect`) for shipping builds |
| `-debug <path>` | No | Path to output debug reflection file with human-readable names (not included in shipping builds) |
| `-spirvcross <path>` | Yes | Path to `spirv-cross` executable |

### Examples

Generate reflection data for a single shader:

```sh
./build/tools/spirv-shader-reflect \
    -spirvcross ./build/tools/spirv-cross \
    -i build/shaderbuild/spirv/sprite_batch_vs.spv \
    -o build/exampleapp/content/shaders/reflect/sprite_batch_vs.reflect
```

Generate with debug output:

```sh
./build/tools/spirv-shader-reflect \
    -spirvcross ./build/tools/spirv-cross \
    -i build/shaderbuild/spirv/sprite_batch_vs.spv \
    -o build/exampleapp/content/shaders/reflect/sprite_batch_vs.reflect \
    -debug build/shaderbuild/reflect_debug/sprite_batch_vs.reflect
```

### Output Formats

This tool produces two distinct FlatBuffers formats controlled by the `-o` and `-debug` flags:

**Shipping output (`-o`)** — Schema: `schemas/shader_reflect.fbs`

Names are stored as 32-bit hashes (`uint32`) for compact size and fast `LookupByKey()` at runtime. This file is included in the game's shipping content archive.

**Debug output (`-debug`)** — Schema: `schemas/shader_reflect_debug.fbs`

Names are stored as human-readable strings for debugging and diagnostics. This file is **not** included in shipping builds.

Both formats contain:

- **Samplers**: Array of `(name, slot)` pairs, sorted by name
- **Cbuffers**: Array of `(name, slot)` pairs, sorted by name
