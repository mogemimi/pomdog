# glsl-rename-combined-samplers

A tool that renames `SPIRV_Cross_Combined*` sampler identifiers in GLSL output back to their original texture names, so that GL sampler uniform names match the names stored in `.reflect` files.

## Background

Slang declares textures and samplers as separate resources (e.g. `Texture2D DiffuseTexture` and `SamplerState TextureSampler`). When `slangc` compiles these to SPIR-V, the resulting binary contains `separate_images` and `separate_samplers` — not combined image samplers.

However, GLSL (ES 300 / Desktop 330) has no concept of separate textures and samplers; it uses combined `sampler2D` uniforms. When `spirv-cross` transpiles the SPIR-V to GLSL, it merges each image+sampler pair into a single `uniform sampler2D` and names it `SPIRV_Cross_Combined{TextureName}{SamplerName}` — for example, `SPIRV_Cross_CombinedDiffuseTextureTextureSampler`.

At runtime, the GL4 backend looks up sampler uniforms by a name hash stored in the `.reflect` file. Because the reflect file records the original texture name (`DiffuseTexture`), not the synthesised combined name, the lookup fails with an error like:

```
slot index not found for sampler: SPIRV_Cross_CombinedDiffuseTextureTextureSampler
```

`glsl-rename-combined-samplers` solves this by renaming every `SPIRV_Cross_Combined*` identifier in the GLSL source back to its original texture name, before the file is minified and shipped.

## How it works

1. Run `spirv-cross --reflect` on the companion SPIR-V file to obtain the `separate_images` and `separate_samplers` JSON arrays
2. Scan the input GLSL file for all `SPIRV_Cross_Combined*` identifiers
3. For each combined name, strip the `SPIRV_Cross_Combined` prefix and match the remainder against known `{ImageName}{SamplerName}` pairs
4. Replace every occurrence of the combined name with the original texture name
5. Write the result to the output file

If no `SPIRV_Cross_Combined*` identifiers are found (e.g. a vertex shader with no samplers), the input is copied unchanged.

## Installation

```sh
cd tools/cmd/glsl-rename-combined-samplers
go build -o ../../../build/tools/glsl-rename-combined-samplers
```

## Usage

```sh
glsl-rename-combined-samplers --spirvcross <path> -spv <file.spv> -i <input.glsl> -o <output.glsl>
```

### Options

| Option                | Description                          |
|-----------------------|--------------------------------------|
| `-i <path>`           | Input GLSL file                      |
| `-o <path>`           | Output GLSL file                     |
| `-spv <path>`         | Companion SPIR-V file (`.spv`)       |
| `--spirvcross <path>` | Path to `spirv-cross` executable     |

### Example

```sh
./build/tools/glsl-rename-combined-samplers \
    --spirvcross ./build/tools/spirv-cross \
    -spv ./build/shaderbuild/spirv/fxaa_ps.spv \
    -i ./build/shaderbuild/glsl300es/fxaa_ps.glsl \
    -o ./build/shaderbuild/glsl300es/fxaa_ps.renamed.glsl
```

Before:

```glsl
uniform sampler2D SPIRV_Cross_CombinedDiffuseTextureTextureSampler;
```

After:

```glsl
uniform sampler2D DiffuseTexture;
```

## Build pipeline integration

`shader-ninja-gen` automatically inserts a `glsl-rename-combined-samplers` step for every GLSL transpilation target. The pipeline becomes:

```
.spv → spirv-cross → .glsl → glsl-rename-combined-samplers → .renamed.glsl → glsl-minifier → .minified.glsl → shipping
```

The renamed GLSL is used by both the GLSL ES 300 (WebGL) and Desktop 330 (OpenGL 4) shipping paths.
