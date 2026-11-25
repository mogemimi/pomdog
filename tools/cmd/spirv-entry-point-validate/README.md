# spirv-entry-point-validate

A static validation tool that checks that a SPIR-V module declares exactly one entry point and that the entry point runs in the expected shader stage. The shader pipeline runs it on every shader before it transpiles the SPIR-V for each backend, so a shader with an ambiguous entry point fails at asset build time rather than at runtime.

The Metal backend takes the only function of each `.metallib` without looking it up by name, and spirv-cross picks the entry point to transpile by stage. A module with more than one entry point would make the function that ships ambiguous, so the tool reports it as an error.

## Features

- **Entry point count**: Fails unless the module declares exactly one `OpEntryPoint`
- **Stage check**: Fails if the execution model of the entry point does not match `-stage`
- **Stamp file output**: Produces a stamp file on success for incremental build integration

## Prerequisites

- No external dependencies. The tool uses the pure Go SPIR-V binary parser in `tools/pkg/spvparse`.

## Installation

```sh
cd tools/cmd/spirv-entry-point-validate
go build -o ../../../build/tools/spirv-entry-point-validate
```

## Usage

```sh
spirv-entry-point-validate -i <in.spv> -stage <vertex|fragment> [-stamp <out.stamp>]
```

### Options

| Option | Required | Description |
|--------|----------|-------------|
| `-i <path>` | Yes | Path to the SPIR-V file |
| `-stage <name>` | Yes | Expected shader stage: `vertex` or `fragment` |
| `-stamp <path>` | No | Path to output stamp file (created only on success) |

### Examples

Validate a vertex shader with stamp file output (for Ninja build integration):

```sh
./build/tools/spirv-entry-point-validate \
    -i build/shaderbuild/spirv/example_vs.renamed.spv \
    -stage vertex \
    -stamp build/shaderbuild/entry_point_validate/example_vs.stamp
```

### Exit codes

| Code | Meaning |
|------|---------|
| 0 | Validation passed |
| 1 | Validation failed (no or several entry points, a stage mismatch, or a parse error) |

### Error examples

Several entry points:

```
error: example_vs.spv: the module declares 2 entry points (vertex 'main', vertex 'other_vs'), want exactly one
```

Stage mismatch:

```
error: example_vs.spv: entry point 'main' is a fragment shader, want a vertex shader
```
