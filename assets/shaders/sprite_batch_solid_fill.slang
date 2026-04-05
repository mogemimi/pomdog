// Copyright mogemimi. Distributed under the MIT license.

struct SpriteBatchConstants {
    float4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__z_} = OutlineWeight
    // {___w} = unused
    float4 DistanceFieldParameters;

    // {rgba} = OutlineColor
    float4 OutlineColor;
};

[[vk::binding(0, 0)]]
ConstantBuffer<SpriteBatchConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

[shader("pixel")]
float4 sprite_batch_solid_fill_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(2)]] float BlendFactor : TEXCOORD1
) : SV_Target0
{
    float4 color = in_Texture.Sample(TextureCoord.xy);
    return lerp(
        color * Color,
        float4(Color.rgb * color.a, color.a),
        BlendFactor);
}
