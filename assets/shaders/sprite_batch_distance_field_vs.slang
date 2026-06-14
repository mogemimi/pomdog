// Copyright mogemimi. Distributed under the MIT license.

struct SpriteBatchConstants {
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<SpriteBatchConstants> cb;

struct VSInput {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    [[vk::location(0)]] float4 PositionTextureCoord;

    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy * sourceRect.{width, height}
    [[vk::location(1)]] float4 Translation;

    // per Instance
    // {xy__} = {rect.xy * inverseTextureSize}
    // {__zw} = {rect.width * inverseTextureSize, rect.height * inverseTextureSize}
    [[vk::location(2)]] float4 SourceRect;

    // per Instance
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = unused (or waterLineYPosition for waterline shader)
    [[vk::location(3)]] float4 OriginRotationWaterLine;

    // per Instance
    // {xyzw} = glyphColor.rgba (Unorm8x4)
    [[vk::location(4)]] float4 Color0;

    // per Instance
    // {xyzw} = outlineColor.rgba (Unorm8x4)
    [[vk::location(5)]] float4 Color1;

    // per Instance
    // {xyzw} = unused (Unorm8x4)
    [[vk::location(6)]] float4 Color2;

    // per Instance
    // {x___} = unused (Unorm8x1)
    // {_y__} = outlineWeight (Unorm8x1)
    // {__z_} = smoothing (Unorm8x1)
    // {___w} = weight (Unorm8x1)
    [[vk::location(7)]] float4 ReservedBlendFactor;
};

[shader("vertex")]
float4 sprite_batch_distance_field_vs(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0,
    [[vk::location(1)]] out float2 TextureCoord : TEXCOORD0,
    [[vk::location(2)]] out float4 OutlineColor : COLOR1,
    [[vk::location(3)]] out float OutlineWeight : TEXCOORD2,
    [[vk::location(4)]] out float Smoothing : TEXCOORD3,
    [[vk::location(5)]] out float Weight : TEXCOORD4
) : SV_Position
{
    float3x3 scaling = float3x3(
        float3(input.Translation.z, 0.0, 0.0),
        float3(0.0, input.Translation.w, 0.0),
        float3(0.0, 0.0, 1.0));

    float cosRotation = cos(input.OriginRotationWaterLine.z);
    float sinRotation = sin(input.OriginRotationWaterLine.z);
    float3x3 rotate = float3x3(
        float3(cosRotation, sinRotation, 0.0),
        float3(-sinRotation, cosRotation, 0.0),
        float3(0.0, 0.0, 1.0));

    float3x3 translate = float3x3(
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(input.Translation.xy, 1.0));

    float3x3 transform = mul(mul(scaling, rotate), translate);
    float3 position = mul(float3(input.PositionTextureCoord.xy - input.OriginRotationWaterLine.xy, 1.0), transform);

    // NOTE: 'ViewProjection' has already been transposed.
    float4 finalPosition = mul(float4(position.xy, 0.0, 1.0), cb.ViewProjection);

    TextureCoord = input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy;
    Color = input.Color0;
    OutlineColor = input.Color1;
    OutlineWeight = input.ReservedBlendFactor.y;
    Smoothing = input.ReservedBlendFactor.z;
    Weight = input.ReservedBlendFactor.w;
    return float4(finalPosition.xy, 0.0, 1.0);
}
