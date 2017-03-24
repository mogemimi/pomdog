#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct __attribute__((__aligned__(256))) MyShaderConstants {
    matrix_float4x4 Model;
    matrix_float4x4 ViewProjection;
};

struct VS_INPUT {
    float3 Position [[attribute(0)]];
    float2 TextureCoord [[attribute(1)]];
};

struct VS_OUTPUT {
    float4 Position [[position]];
    float2 TextureCoord;
};

// Vertex shader function
vertex VS_OUTPUT SimpleEffectVS(
    VS_INPUT in [[stage_in]],
    constant MyShaderConstants& uniforms [[buffer(0)]])
{
    VS_OUTPUT out;

    float4 position = float4(in.Position, 1.0);
    out.Position = ((position * uniforms.Model) * uniforms.ViewProjection);
    out.TextureCoord = in.TextureCoord.xy;
    return out;
}

// Fragment shader function
fragment half4 SimpleEffectPS(
    VS_OUTPUT in [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]])
{
    float4 color = diffuseTexture.sample(textureSampler, in.TextureCoord);
    return half4(color);
}
