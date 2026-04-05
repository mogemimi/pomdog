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
float4 sprite_batch_distance_field_outline_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(2)]] float BlendFactor : TEXCOORD1
) : SV_Target0
{
    float smoothing = cb.DistanceFieldParameters.x;
    float weight = cb.DistanceFieldParameters.y;
    float outlineWeight = cb.DistanceFieldParameters.z;

    // NOTE: Use R8 texture (not A8 texture)
    float distance = in_Texture.Sample(TextureCoord.xy).r;

    float outlineFactor = smoothstep(outlineWeight - smoothing, outlineWeight + smoothing, distance);
    float opacity = smoothstep(weight - smoothing, weight + smoothing, distance);

    float4 color = lerp(cb.OutlineColor, Color, outlineFactor);

    return float4(color.rgb, color.a * opacity);
}
