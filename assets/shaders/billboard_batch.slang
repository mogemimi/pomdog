// Copyright mogemimi. Distributed under the MIT license.

struct WorldConstants {
    float4x4 ViewProjection;
    float4x4 View;
    float4x4 Projection;
    float4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    float4 LightDirection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<WorldConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D diffuseTexture;

struct VSInput {
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    [[vk::location(0)]] float4 PositionTextureCoord;

    // per Instance
    // {xyz_} = position.xyz
    // {___w} = rotation.z
    [[vk::location(1)]] float4 Translation;

    // {xy__} = textureCoord.xy
    // {__zw} = textureSize.xy
    [[vk::location(2)]] float4 TextureCoordRect;

    // {xy__} = originPivot.xy
    // {__zw} = scale.xy
    [[vk::location(3)]] float4 OriginPivotScale;

    // {xyz_} = color.rgb
    // {___w} = color.a
    [[vk::location(4)]] float4 Color;
};

[shader("vertex")]
float4 billboard_batch_vs(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0,
    [[vk::location(1)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    float2 scale = input.OriginPivotScale.zw;
    float4x4 scaling = float4x4(
        float4(scale.x, 0.0, 0.0, 0.0),
        float4(0.0, scale.y, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float rotationZ = input.Translation.w;
    float cosRotation = cos(rotationZ);
    float sinRotation = sin(rotationZ);
    float4x4 rotate = float4x4(
        float4(cosRotation, sinRotation, 0.0, 0.0),
        float4(-sinRotation, cosRotation, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float4x4 transform = mul(rotate, scaling);
    float4 worldSpacePosition = mul(transform, float4(input.PositionTextureCoord.xy - input.OriginPivotScale.xy, 0.0, 1.0));
    float4 viewSpaceOffset = mul(cb.View, float4(input.Translation.xyz, 1.0));

    TextureCoord = input.PositionTextureCoord.zw * input.TextureCoordRect.zw + input.TextureCoordRect.xy;
    Color = input.Color;
    return mul(cb.Projection, float4(worldSpacePosition.xyz + viewSpaceOffset.xyz, 1.0));
}

[shader("pixel")]
float4 billboard_batch_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 color = diffuseTexture.Sample(TextureCoord.xy);
    return color * Color;
}
