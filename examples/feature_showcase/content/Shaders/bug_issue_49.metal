#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;
constant int PomdogVertexBufferSlotOffset = 20;

struct __attribute__((__aligned__(256))) MyShaderConstants {
    matrix_float4x4 ViewProjection;
};

struct VertexShaderInput {
    float3 Position [[attribute(0)]];
    float2 TextureCoord [[attribute(1)]];
};

struct InstanceVertex {
    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation [[attribute(2)]];

    // {xyzw} = color.rgba
    float4 Color [[attribute(3)]];
};

struct VertexShaderOutput {
    float4 Position [[position]];
    float2 TextureCoord;
    float4 Color;
};

vertex VertexShaderOutput BugIssue49_VS(
    VertexShaderInput in [[stage_in]],
    constant InstanceVertex* instanceVertices [[buffer(1 + PomdogVertexBufferSlotOffset)]],
    constant MyShaderConstants& uniforms [[buffer(0)]],
    ushort vertexIndex [[vertex_id]],
    ushort instanceIndex [[instance_id]])
{
    constant InstanceVertex& perInstance = instanceVertices[instanceIndex];

    VertexShaderOutput out;

    float4x4 scaling = float4x4(
        float4(perInstance.Translation.z, 0.0, 0.0, 0.0),
        float4(0.0, perInstance.Translation.w, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float4x4 translate = float4x4(
        float4(1.0, 0.0, 0.0, 0.0),
        float4(0.0, 1.0, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(perInstance.Translation.x, perInstance.Translation.y, 0.0, 1.0));

    float4x4 transform = translate * scaling;

    float4 worldSpacePosition = transform * float4(in.Position, 1.0);
    out.Position = uniforms.ViewProjection * worldSpacePosition;
    out.TextureCoord = in.TextureCoord.xy;
    out.Color = perInstance.Color;
    return out;
}

fragment half4 BugIssue49_PS(
    VertexShaderOutput in [[stage_in]],
    texture2d<float> diffuseTexture [[texture(2)]], // NOTE: not [[texture(0)]] to reproduce the issue
    sampler textureSampler [[sampler(2)]])
{
    float4 color = diffuseTexture.sample(textureSampler, in.TextureCoord);
    return half4(in.Color * color);
}
