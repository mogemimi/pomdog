# shader-archive-gen

A code generator that produces archive recipe TOML files (`archive_shaders.toml`) from `shaderbuild.toml` shader build recipes. It automates the creation of per-platform shader file lists for the `archive-content` tool, eliminating the need to manually maintain shader entries in archive recipes as the number of shaders and target platforms grows.

## Features

- **Automatic shader enumeration**: Reads `[[compile_vs]]` and `[[compile_ps]]` entries from `shaderbuild.toml` and generates matching archive paths for all shader backends
- **Per-platform grouping**: Produces separated `[[group]]` blocks with `target_platforms` for desktop GLSL, WebGL, and DXBC
- **Multi-recipe support**: Accepts multiple `shaderbuild.toml` files as positional arguments and merges them into a single archive recipe
- **Deterministic output**: Shader names are sorted alphabetically for stable, diff-friendly output

## Installation

```sh
cd tools/cmd/shader-archive-gen
go build -o ../../../build/tools/shader-archive-gen
```

## Usage

```sh
shader-archive-gen -o <output.toml> <shaderbuild.toml> [shaderbuild2.toml ...]
```

### Options

| Option | Description |
|--------|-------------|
| `-o <path>` | Output archive recipe file (`.toml`). **Required** |

### Examples

Generate the engine shader archive recipe:

```sh
./build/tools/shader-archive-gen \
    -o ./assets/archive/archive_shaders.toml \
    ./assets/shaders/shaderbuild.toml
```

Generate a combined archive recipe from both engine and example shaders:

```sh
./build/tools/shader-archive-gen \
    -o ./assets/archive/archive_shaders.toml \
    ./assets/shaders/shaderbuild.toml \
    ./examples/feature_showcase/assets/shaders/shaderbuild.toml
```

## Output Format

The generated TOML file contains `[[group]]` entries compatible with `archive-content`:

```toml
[[group]]
paths = [
    "shaders/reflect/basic_effect_ps.reflect",
    "shaders/reflect/basic_effect_vs.reflect",
]

[[group]]
target_platforms = ["windows", "linux", "macos"]
paths = [
    "shaders/glsl/basic_effect_ps.glsl",
    "shaders/glsl/basic_effect_vs.glsl",
]

[[group]]
target_platforms = ["emscripten"]
paths = [
    "shaders/webgl/basic_effect_ps.glsl",
    "shaders/webgl/basic_effect_vs.glsl",
]

[[group]]
target_platforms = ["windows"]
paths = [
    "shaders/d3d11/basic_effect_ps.dxbc",
    "shaders/d3d11/basic_effect_vs.dxbc",
]
```
