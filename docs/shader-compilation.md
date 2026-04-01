# Shader Compilation

This document covers the shader compilation pipeline in detail — the source language, build toolchain, cross-compilation to multiple graphics backends, and known pitfalls.

For an overview of the full asset pipeline, see [Asset Pipeline and Runtime](asset-pipeline-and-runtime.md).

## Source Language: Slang

All shaders are written in [Slang](https://github.com/shader-slang/slang) (`.slang` files). Slang is a shader language with module and generics support that compiles to SPIR-V as an intermediate representation. Using Slang as the single source language eliminates the need to maintain separate GLSL, HLSL, and Metal shaders.

Source files live in `assets/shaders/` for engine-level shaders and `examples/<app>/assets/shaders/` for per-application shaders.

## Build Recipe: `shaderbuild.toml`

Each shader directory contains a `shaderbuild.toml` that declares compilation units and link groups:

```toml
[[compile_vs]]
name = "basic_effect_vs"
source = "basic_effect.slang"
entrypoint = "basic_effect_vs"

[[compile_ps]]
name = "basic_effect_ps"
source = "basic_effect.slang"
entrypoint = "basic_effect_ps"
vsout = "basic_effect_vs"

[[link]]
name = "basic_effect"
vs = "basic_effect_vs"
ps = "basic_effect_ps"
```

- `[[compile_vs]]` and `[[compile_ps]]` tables encode the shader stage directly in the table name.
- The optional `vsout` field on `[[compile_ps]]` entries tells the pipeline to run `spirv-patch-interface` to restore any VS outputs eliminated by slangc's dead-code elimination (see [Dead-Code Elimination of Varyings](#dead-code-elimination-of-varyings)).

## Build Toolchain

The tool `shader-ninja-gen` reads `shaderbuild.toml` and generates a [Ninja](https://ninja-build.org/) build file (`build.ninja`) that compiles and cross-compiles shaders for every target backend.

| Tool | Role |
|------|------|
| `slangc` | Compile `.slang` → SPIR-V |
| `spirv-cross` | Transpile SPIR-V → GLSL / HLSL / Metal |
| `spirv-shader-reflect` | Extract reflection metadata (UBO names, binding slots) from SPIR-V |
| `spirv-link-validate` | Statically verify VS/PS interface (varying) consistency |
| `spirv-rename-blocks` | Strip `_std140` suffix from UBO type names in SPIR-V |
| `spirv-patch-interface` | Restore PS input variables removed by slangc dead-code elimination |
| `spirv-strip-debug` | Strip debug instructions from Vulkan SPIR-V output |
| `glsl-rename-combined-samplers` | Rewrite spirv-cross combined sampler names back to original texture names |
| `ninja` | Execute the generated build file |

## Multi-Backend Transpilation

| Target | Format | Tool |
|--------|--------|------|
| Vulkan | SPIR-V | `slangc` (native output) |
| OpenGL Desktop | GLSL 4.10 | `spirv-cross` |
| WebGL / Emscripten | GLSL ES 3.00 | `spirv-cross` |
| Metal (macOS/iOS) | MSL 2.0 | `spirv-cross` |
| Direct3D 11 | DXBC | `spirv-cross` → HLSL SM4.0 → `fxc.exe` |
| Direct3D 12 | DXIL | `spirv-cross` → HLSL SM6.0 → `dxc.exe` |

The spirv-cross flags used for each target:

```sh
# Desktop GLSL (OpenGL 4.1)
spirv-cross --no-es --version 410 --remove-unused-variables --no-420pack-extension

# ES 300 (WebGL 2 / Emscripten)
spirv-cross --es --version 300 --remove-unused-variables

# Metal
spirv-cross --msl --msl-version 20100

# HLSL (Direct3D 11)
spirv-cross --hlsl --shader-model 40

# HLSL (Direct3D 12)
spirv-cross --hlsl --shader-model 60
```

> **Note:** Desktop GLSL uses `--version 410`, not `--version 330`. GLSL 3.30 does not support `layout(location)` on varyings, requiring name-based matching which is fragile.

The generated Ninja file is then executed by the bundled `ninja` binary (`build/tools/ninja`).

## SPIR-V Post-Processing

Before transpilation, the pipeline applies several SPIR-V rewriting passes:

1. **`spirv-rename-blocks`** — Strips the `_std140` suffix that slangc appends to UBO type names (e.g. `SpriteBatchConstants_std140` → `SpriteBatchConstants`). This ensures constant buffer names in transpiled code match the C++ engine's expectations.
2. **`spirv-patch-interface`** — For pixel shaders with a `vsout` reference, restores VS output variables that slangc eliminated via dead-code removal. This prevents semantic gaps (e.g. missing `TEXCOORD1`) in HLSL output.
3. **`spirv-strip-debug`** — Strips debug instructions (`OpName`, `OpSource`, `OpLine`, etc.) and `SPV_KHR_non_semantic_info` metadata. Applied only to the Vulkan SPIR-V output path, after all spirv-cross transpilations are complete.

## GLSL Combined Image-Sampler Renaming

When `spirv-cross` transpiles SPIR-V to GLSL, it may produce **combined image-sampler** identifiers that merge the texture and sampler names into a single prefixed name:

```glsl
// spirv-cross output — combined name
uniform sampler2D SPIRV_Cross_CombinedDiffuseTextureTextureSampler;
```

The pomdog engine's OpenGL backend resolves sampler bind slots by computing xxHash-32 of the **original** texture name (e.g. `DiffuseTexture`). Because `SPIRV_Cross_CombinedDiffuseTextureTextureSampler` does not match the original name, the slot lookup fails at runtime.

The `glsl-rename-combined-samplers` tool post-processes GLSL output to rewrite combined names back to the original texture name:

```glsl
// After glsl-rename-combined-samplers
uniform sampler2D DiffuseTexture;
```

The tool runs after spirv-cross in the Ninja pipeline:

```
.spv → spirv-cross → .glsl → glsl-rename-combined-samplers → .glsl (final)
```

Additionally, `spirv-shader-reflect` extracts texture bindings from `OpDecorate` instructions for `separate_images` (rather than combined image-samplers), ensuring the reflect data contains the original texture name and its binding index.

> **Note:** This issue only affects the OpenGL / WebGL backends. HLSL, Metal, and Vulkan do not depend on GLSL uniform names.

## Shader Reflection

After compilation, `spirv-shader-reflect` extracts binding metadata from SPIR-V bytecode and writes `.reflect` files. These files use a FlatBuffers schema (`schemas/shader_reflect.fbs`) to record constant-buffer and sampler slot assignments:

```flatbuffers
table ConstantBufferSlot {
    name : uint32(key);   // xxHash-32 of the UBO name
    slot : uint8;         // SPIR-V binding index
}

table SamplerSlot {
    name : uint32(key);   // xxHash-32 of the texture/sampler name
    slot : uint8;         // SPIR-V binding index
}

table ShaderReflect {
    cbuffers : [ConstantBufferSlot];
    samplers : [SamplerSlot];
}
```

A debug variant (`shader_reflect_debug.fbs`) stores human-readable names for development diagnostics.

## Link Validation

The `-link-validate` flag on `shader-ninja-gen` runs `spirv-link-validate`, which checks that vertex shader outputs match pixel shader inputs when a `[[link]]` group is defined. This catches interface mismatches at build time.

When a warning such as `"VS output 'BlendFactor' (float4) at location 1 is not consumed by PS"` appears, it indicates dead-code elimination has removed a varying — see below.

## Slang Pitfalls and Workarounds

This section documents known issues and workarounds encountered when using Slang with spirv-cross transpilation.

### Row-Major vs Column-Major Matrix Constructors

GLSL and Slang interpret matrix constructor arguments differently:

| Language | `mat3(v0, v1, v2)` interpretation |
|----------|-----------------------------------|
| **GLSL** | `v0, v1, v2` are **column vectors** (column-major) |
| **Slang/HLSL** | `v0, v1, v2` are **row vectors** (row-major) |

When converting GLSL shaders to Slang, the constructor arguments must be transposed. When in doubt, compare against the existing HLSL version of the same shader — HLSL and Slang share the same row-major convention.

### Varyings Must Use Flat Parameters, Not Struct Returns

Returning VS outputs as a struct causes spirv-cross to generate mismatched varying names between VS and PS:

```
VS: entryPointParam_main.Color   ← different name
PS: input.Color                   ← different name
```

GLSL ES 3.00 (WebGL 2) matches varyings by **name**, so a mismatch breaks the VS-PS link.

**Solution**: Use `out` parameters instead of struct returns:

```slang
[shader("vertex")]
float4 main(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0,
    [[vk::location(1)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
```

### ConstantBuffer and Sampler Binding Collisions

When a shader uses both `ConstantBuffer` and `Sampler2D`, giving both `[[vk::binding(0)]]` produces a Slang warning. Separate them by descriptor set:

```slang
[[vk::binding(0, 0)]]  // binding=0, set=0
ConstantBuffer<MyConstants> cb;

[[vk::binding(0, 1)]]  // binding=0, set=1
Sampler2D myTexture;
```

spirv-cross maps these correctly to each backend's namespace (e.g. HLSL `register(b0)` / `register(t0)`, Metal `buffer(0)` / `texture(0)`).

### `_std140` UBO Name Suffix

When slangc compiles to SPIR-V, it appends `_std140` to ConstantBuffer type names (e.g. `SpriteBatchConstants_std140`). The `spirv-rename-blocks` tool strips this suffix so that transpiled code and C++ engine code use matching names.

### `layout(row_major)` in Generated GLSL

Slang treats matrices as row-major by default. The generated GLSL UBO members include `layout(row_major)` qualifiers, and ES 300 output wraps matrix access in `spvWorkaroundRowMajor()` helper functions. This does not affect correctness as long as the CPU-side data matches the expected layout.

### Dead-Code Elimination of Varyings

Slang's dead-code elimination removes PS input variables that are not referenced in the pixel shader body. This creates **gaps** in varying locations between VS and PS, which can cause display corruption on Direct3D 11.

**Example**: If VS outputs `Color` (location 0), `BlendFactor` (location 1), and `TextureCoord` (location 2), but PS does not use `BlendFactor`, slangc removes location 1 from the PS SPIR-V. The resulting HLSL has `TEXCOORD0` and `TEXCOORD2` with no `TEXCOORD1`, causing a semantic gap.

**Solution**: The `spirv-patch-interface` tool compares VS output and PS input interfaces, then injects missing input variables into the PS SPIR-V:

```
.slang → slangc → .spv → spirv-patch-interface → .patched.spv → spirv-cross → GLSL/HLSL/Metal
```

This is controlled by the `vsout` field in `shaderbuild.toml`. The `spirv-link-validate` tool detects these gaps at build time.

> **Note:** This cannot be prevented with slangc compiler flags (`-O0`, `-preserve-params`, etc.) — the dead-code elimination occurs during IR lowering, not as an optimization pass.

## GLSL → Slang Conversion Reference

### Type Mappings

| GLSL | Slang |
|------|-------|
| `vec2` / `vec3` / `vec4` | `float2` / `float3` / `float4` |
| `mat3x3` / `mat4x4` | `float3x3` / `float4x4` |
| `ivec2` | `int2` |
| `sampler2D` | `Sampler2D` |

### Function Mappings

| GLSL | Slang |
|------|-------|
| `texture(tex, uv)` | `tex.Sample(uv)` |
| `mix(a, b, t)` | `lerp(a, b, t)` |
| `v * M` | `mul(v, M)` |
| `M * v` | `mul(M, v)` |

> **Important:** Slang does not support the `*` operator for matrix-vector multiplication. Always use `mul()`.

### UBO Syntax

```slang
struct SpriteBatchConstants {
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<SpriteBatchConstants> cb;
```

### Texture / Sampler Syntax

```slang
[[vk::binding(0, 1)]]
Sampler2D myTexture;
```

### Entry Points

```slang
[shader("vertex")]
float4 vertexMain(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0
) : SV_Position
{
    Color = input.Color;
    return mul(float4(input.Position.xy, 0.0, 1.0), cb.ViewProjection);
}

[shader("pixel")]
PSOutput pixelMain(
    [[vk::location(0)]] float4 Color : COLOR0
)
{
    PSOutput output;
    output.FragColor = Color;
    return output;
}
```
