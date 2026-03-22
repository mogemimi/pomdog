# spirv-strip-debug

A tool that strips debug instructions and non-semantic extended instructions from a SPIR-V binary, producing a smaller module suitable for shipping.

## Background

Slang (`slangc`) and other SPIR-V compilers embed debug information — symbol names (`OpName`, `OpMemberName`), source locations (`OpLine`, `OpNoLine`), and source text (`OpSource`, `OpString`) — in the output binary. The `SPV_KHR_non_semantic_info` extension adds further non-functional metadata via `OpExtInstImport` / `OpExtInst` instructions.

This debug information is useful during development (e.g. for spirv-cross to produce readable transpiled code, or for GPU debuggers) but is unnecessary in shipping Vulkan SPIR-V binaries. Stripping it reduces file size without affecting runtime behaviour.

In the pomdog pipeline, all spirv-cross transpilation (GLSL, HLSL, Metal) is performed **before** stripping, so those backends retain human-readable names. Only the shipped Vulkan SPIR-V is stripped.

## What is stripped

### `--strip-debug`

Removes the following SPIR-V debug instructions (Section 7.a of the SPIR-V spec):

| Opcode | Instruction |
|--------|-------------|
| 2 | `OpSourceContinued` |
| 3 | `OpSource` |
| 4 | `OpSourceExtension` |
| 5 | `OpName` |
| 6 | `OpMemberName` |
| 7 | `OpString` |
| 8 | `OpLine` |
| 317 | `OpNoLine` |
| 330 | `OpModuleProcessed` |

### `--strip-nonsemantic`

Removes `SPV_KHR_non_semantic_info` instructions:

- `OpExtInstImport` entries whose name starts with `"NonSemantic."` (e.g. `NonSemantic.Debuginfo.100`)
- All `OpExtInst` calls that reference those instruction sets

## Installation

```sh
cd tools/cmd/spirv-strip-debug
go build -o ../../../build/tools/spirv-strip-debug
```

## Usage

```sh
spirv-strip-debug -i <input.spv> -o <output.spv> [--strip-debug] [--strip-nonsemantic]
```

At least one of `--strip-debug` or `--strip-nonsemantic` must be specified.

### Options

| Option                | Description                                               |
|-----------------------|-----------------------------------------------------------|
| `-i <path>`           | Input SPIR-V file path                                    |
| `-o <path>`           | Output SPIR-V file path                                   |
| `--strip-debug`       | Strip debug instructions (OpName, OpSource, OpLine, etc.) |
| `--strip-nonsemantic` | Strip SPV_KHR_non_semantic_info instructions              |
| `-v`                  | Print strip logs to stdout                                |

### Example

```sh
./build/tools/spirv-strip-debug \
    --strip-debug \
    --strip-nonsemantic \
    -i ./build/shaderbuild/spirv/sprite_batch_vs.spv \
    -o ./build/shaderbuild/spirv/sprite_batch_vs.stripped.spv \
    -v
```

When `-v` is specified and instructions are stripped, the tool prints the number of instructions removed to stdout:

```text
stripped 25 instructions
```

If no instructions are stripped, there is no output.

## Build pipeline integration

`shader-ninja-gen` automatically inserts a `strip_spirv_debug` step for every shader in the Vulkan output path. The stripping happens after all spirv-cross transpilations, so only the shipped Vulkan binary is affected:

```
.spv ─► rename ─► patch ─┬─► spirv-cross (GLSL/HLSL/Metal)
                         │   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                         │      retains debug names
                         │
                         └─► strip_spirv_debug ─► .stripped.spv ─► copy ─► vk/*.spv
                             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                removes debug info
```

The Ninja rule uses both flags:

```
spirv-strip-debug --strip-debug --strip-nonsemantic -i $in -o $out
```
