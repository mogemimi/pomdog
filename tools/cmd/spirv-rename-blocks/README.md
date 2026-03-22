# spirv-rename-blocks

A tool that rewrites `OpName` instructions in a SPIR-V binary to strip a configurable suffix from uniform block type names.

## Background

When `slangc` compiles Slang source to SPIR-V, it appends `_std140` to the type names of uniform buffer objects (UBOs). For example, a constant buffer declared as `SpriteBatchConstants` produces the SPIR-V type name `SpriteBatchConstants_std140`.

When `spirv-cross` transpiles this SPIR-V to HLSL, GLSL, or Metal, the mangled name propagates into the output. The C++ engine would then need to bind constant buffers using `"SpriteBatchConstants_std140"` instead of the clean name `"SpriteBatchConstants"`.

`spirv-rename-blocks` solves this by stripping the suffix from `OpName` instructions in the SPIR-V binary before any downstream tool processes it. Since pomdog resolves constant buffer bindings by xxHash-32 of the name (via `.reflect` files), the SPIR-V name must match the name the C++ code expects.

## How it works

1. Parse the SPIR-V binary into a word stream using `tools/pkg/spvparse`
2. Walk all instructions, looking for `OpName` (opcode 5)
3. For each `OpName` whose string operand ends with the target suffix (default `_std140`):
   - Trim the suffix
   - Re-encode the shortened name into SPIR-V word operands
   - Rebuild the `OpName` instruction with the correct word count
4. Write the modified SPIR-V to the output file

If no names match the suffix, the file is copied unchanged.

## Installation

```sh
cd tools/cmd/spirv-rename-blocks
go build -o ../../../build/tools/spirv-rename-blocks
```

## Usage

```sh
spirv-rename-blocks -i <input.spv> -o <output.spv> [-suffix <suffix>]
```

### Options

| Option          | Description                                                     |
|-----------------|-----------------------------------------------------------------|
| `-i <path>`     | Input SPIR-V file path                                          |
| `-o <path>`     | Output SPIR-V file path                                         |
| `-suffix <str>` | Suffix to strip from `OpName` instructions (default: `_std140`) |
| `-v`            | Print rename logs to stdout                                     |

### Example

```sh
./build/tools/spirv-rename-blocks \
    -i ./build/shaderbuild/spirv/sprite_batch_vs.spv \
    -o ./build/shaderbuild/spirv/sprite_batch_vs.renamed.spv \
    -v
```

When `-v` is specified and names are renamed, the tool prints messages to stdout:

```text
renamed: "SpriteBatchConstants_std140" -> "SpriteBatchConstants" (ID 42)
```

## Build pipeline integration

`shader-ninja-gen` inserts a `rename_spirv_blocks` step immediately after `slangc` compilation for all Slang-sourced shaders. The renamed SPIR-V is then used by all downstream steps (patch, transpile, strip, reflect):

```
.slang → slangc → .spv → spirv-rename-blocks → .renamed.spv → patch / transpile / strip / reflect
```

The Ninja rule:

```sh
spirv-rename-blocks -i $in -o $out
```
