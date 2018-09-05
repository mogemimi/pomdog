#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;
constant int PomdogVertexBufferSlotOffset = 20;

struct VS_INPUT {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    float4 PositionTextureCoord [[attribute(0)]];
};

struct InstanceVertex {
    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation [[attribute(1)]];

    // {xy__} = {rect.xy}
    // {__zw} = {rect.width, rect.height}
    float4 SourceRect [[attribute(2)]];

    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = layerDepth
    float4 OriginRotationDepth [[attribute(3)]];

    // {xyzw} = color.rgba
    float4 Color [[attribute(4)]];

    // {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
    // {__zw} = unused
    float2 InverseTextureSize [[attribute(5)]];
};

struct VS_OUTPUT {
    float4 Position [[position]];
    float4 Color;
    float2 TextureCoord;
};

struct __attribute__((__aligned__(256))) SpriteBatchConstants {
    matrix_float4x4 ViewProjection;
};

vertex VS_OUTPUT SpriteBatchVS(
    VS_INPUT                       input            [[stage_in]],
    constant InstanceVertex*       instanceVertices [[buffer(1 + PomdogVertexBufferSlotOffset)]],
    constant SpriteBatchConstants& uniforms         [[buffer(0)]],
    ushort                         vertexIndex      [[vertex_id]],
    ushort                         instanceIndex    [[instance_id]])
{
    constant InstanceVertex& perInstance = instanceVertices[instanceIndex];

    matrix_float3x3 scaling = matrix_float3x3(
        float3(perInstance.Translation.z * perInstance.SourceRect.z, 0.0, 0.0),
        float3(0.0, perInstance.Translation.w * perInstance.SourceRect.w, 0.0),
        float3(0.0, 0.0, 1.0));

    float cosRotation = cos(perInstance.OriginRotationDepth.z);
    float sinRotation = sin(perInstance.OriginRotationDepth.z);
    matrix_float3x3 rotate = matrix_float3x3(
        float3(cosRotation, -sinRotation, 0.0),
        float3(sinRotation, cosRotation, 0.0),
        float3(0.0, 0.0, 1.0));

    matrix_float3x3 translate = matrix_float3x3(
        float3(1.0, 0.0, perInstance.Translation.x),
        float3(0.0, 1.0, perInstance.Translation.y),
        float3(0.0, 0.0, 1.0));

    matrix_float3x3 transform = (scaling * rotate) * translate;
    float3 position = float3(input.PositionTextureCoord.xy - perInstance.OriginRotationDepth.xy, 1.0) * transform;

    float4 finalPosition = float4(position.xy, 0.0, 1.0) * uniforms.ViewProjection;

    VS_OUTPUT output;
    output.Position = float4(finalPosition.xy, perInstance.OriginRotationDepth.w, 1.0);
    output.TextureCoord = ((input.PositionTextureCoord.zw * perInstance.SourceRect.zw)
        + perInstance.SourceRect.xy) * perInstance.InverseTextureSize.xy;
    output.Color = perInstance.Color;

    return output;
}

fragment half4 SpriteBatchPS(
    VS_OUTPUT        input          [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]])
{
    float4 color = diffuseTexture.sample(textureSampler, input.TextureCoord.xy);
    return half4(color * input.Color);
}
