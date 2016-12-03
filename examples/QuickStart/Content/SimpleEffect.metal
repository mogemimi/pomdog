#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct __attribute__((__aligned__(256)))
{
    matrix_float4x4 Model;
    matrix_float4x4 ViewProjection;
} MyShaderConstants;

typedef struct
{
    float3 Position [[attribute(0)]];
    float2 TextureCoord [[attribute(1)]];
} VS_INPUT;

typedef struct {
    float4 Position [[position]];
    float2 TextureCoord;
} VS_OUTPUT;

// Vertex shader function
vertex VS_OUTPUT SimpleEffectVS(
    VS_INPUT in [[stage_in]],
    constant MyShaderConstants& uniforms [[ buffer(1) ]])
{
    VS_OUTPUT out;

    float4 position = float4(in.Position, 1.0);
    out.Position = (uniforms.ViewProjection * (uniforms.Model * position));
    out.TextureCoord = in.TextureCoord.xy;
    return out;
}

// Fragment shader function
fragment half4 SimpleEffectPS(
    VS_OUTPUT in [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]])
{
    constexpr sampler sampleLinear(filter::linear);
    float4 color = diffuseTexture.sample(sampleLinear, in.TextureCoord);
    return half4(color);
}
