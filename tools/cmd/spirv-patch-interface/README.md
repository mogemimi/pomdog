# spirv-patch-interface

A tool that patches a fragment shader SPIR-V binary to restore input variables eliminated by the compiler's dead-code elimination.

## Background

Slang (`slangc`) removes unused entry-point parameters from SPIR-V during IR lowering. When a pixel shader declares an input variable (e.g. `BlendFactor`) that it does not reference in its body, slangc strips it from the SPIR-V output. This is not an optimisation pass — no compiler flag (`-O0`, `-preserve-params`, `-minimum-slang-optimization`) prevents it.

The missing input causes a gap in `TEXCOORD` semantics when `spirv-cross` transpiles the SPIR-V to HLSL. For example, the PS input struct becomes `{ Color : TEXCOORD0; TextureCoord : TEXCOORD2; }` with `TEXCOORD1` missing. On D3D11, this gap causes incorrect data routing from the vertex shader, breaking rendering.

`spirv-patch-interface` solves this by comparing the VS output interface with the PS input interface and injecting any missing variables back into the PS SPIR-V binary, before `spirv-cross` processes it.

## How it works

1. Parse both the VS and PS SPIR-V modules using `tools/pkg/spvparse`
2. Collect VS output variables and PS input variables by `location`
3. Identify VS outputs that have no corresponding PS input (the "missing" variables)
4. For each missing variable, inject into the PS SPIR-V:
   - `OpName` (debug name)
   - `OpDecorate` (location decoration)
   - `OpTypePointer` / `OpTypeVector` / `OpTypeFloat` if not already present
   - `OpVariable` (Input storage class)
   - Extend `OpEntryPoint` interface list with the new variable ID
5. Write the patched SPIR-V to the output file

If no variables are missing, the PS SPIR-V is copied unchanged.

## Installation

```sh
cd tools/cmd/spirv-patch-interface
go build -o ../../../build/tools/spirv-patch-interface
```

## Usage

```sh
spirv-patch-interface -vs <vs.spv> -ps <ps.spv> -o <out.spv>
```

### Options

| Option       | Description                         |
|--------------|-------------------------------------|
| `-vs <path>` | Path to vertex shader SPIR-V file   |
| `-ps <path>` | Path to fragment shader SPIR-V file |
| `-o <path>`  | Output patched SPIR-V file path     |
| `-v`         | Print patch logs to stdout          |

### Example

```sh
./build/tools/spirv-patch-interface \
    -vs ./build/shaderbuild/spirv/sprite_batch_vs.spv \
    -ps ./build/shaderbuild/spirv/sprite_batch_distance_field_ps.spv \
    -o ./build/shaderbuild/spirv/sprite_batch_distance_field_ps.patched.spv \
    -v
```

When `-v` is specified and patching is applied, the tool prints messages to stdout:

```text
patched: added PS input 'BlendFactor' (float4) at location 1
```

## Build pipeline integration

`shader-ninja-gen` automatically inserts a `spirv-patch-interface` step for any `[[compile_ps]]` entry that specifies a `vsout` field. The pipeline becomes:

```
.slang → slangc → .spv → spirv-patch-interface → .patched.spv → spirv-cross → GLSL/HLSL/Metal
```

The patched SPIR-V is used by all downstream transpilation steps (GLSL, HLSL, Metal, Vulkan copy, and reflection).

## Supported types

The tool handles the following varying types: `float`, `float2`, `float3`, `float4`. If the required base type or pointer type does not already exist in the PS SPIR-V module, the tool creates the necessary type instructions.
