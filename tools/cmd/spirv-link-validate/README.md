# spirv-link-validate

A static validation tool that checks the interface compatibility between vertex shader and pixel shader SPIR-V modules. It verifies that VS outputs and PS inputs match by location and type, catching linking errors at build time rather than at runtime.

## Features

- **Location-based matching**: Validates that every PS input location has a corresponding VS output
- **Type checking**: Ensures matching locations have compatible types (e.g., `float2` vs `float4`)
- **Unconsumed output warnings**: Reports VS outputs not consumed by the PS (non-fatal)
- **Builtin filtering**: Automatically skips built-in variables (`SV_Position`, `gl_FragCoord`, etc.)
- **Struct flattening**: Handles both flat interface variables and struct-based interface blocks
- **Stamp file output**: Produces a stamp file on success for incremental build integration

## Prerequisites

- No external dependencies. The tool includes a pure Go SPIR-V binary parser.

## Installation

```sh
cd tools/cmd/spirv-link-validate
go build -o ../../../build/tools/spirv-link-validate
```

## Usage

```sh
spirv-link-validate -vs <vs.spv> -ps <ps.spv> [-stamp <out.stamp>]
```

### Options

| Option | Required | Description |
|--------|----------|-------------|
| `-vs <path>` | Yes | Path to the vertex shader SPIR-V file |
| `-ps <path>` | Yes | Path to the pixel/fragment shader SPIR-V file |
| `-stamp <path>` | No | Path to output stamp file (created only on success) |

### Examples

Validate a shader pair:

```sh
./build/tools/spirv-link-validate \
    -vs build/shaderbuild/spirv/example_vs.spv \
    -ps build/shaderbuild/spirv/example_ps.spv
```

Validate with stamp file output (for Ninja build integration):

```sh
./build/tools/spirv-link-validate \
    -vs build/shaderbuild/spirv/example_vs.spv \
    -ps build/shaderbuild/spirv/example_ps.spv \
    -stamp build/shaderbuild/link_validate/example.stamp
```

### Exit codes

| Code | Meaning |
|------|---------|
| 0 | Validation passed |
| 1 | Validation failed (missing locations, type mismatches, or parse errors) |

### Error examples

Missing VS output:

```
error: PS input 'TextureCoord' at location 0 has no matching VS output
link validation failed: 1 error(s)
```

Type mismatch:

```
error: type mismatch at location 0: VS output 'TextureCoord' is float4, PS input 'TextureCoord' is float2
link validation failed: 1 error(s)
```
