// Copyright mogemimi. Distributed under the MIT license.

struct SpriteBatchConstants {
    float4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    float4 DistanceFieldParameters;
};

[[vk::binding(0, 0)]]
ConstantBuffer<SpriteBatchConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

[shader("pixel")]
float4 sprite_batch_distance_field_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float4 BlendFactor : COLOR1,
    [[vk::location(2)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float smoothing = cb.DistanceFieldParameters.x;
    float weight = cb.DistanceFieldParameters.y;

    // NOTE: Use R8 texture (not A8 texture)
    float distance = in_Texture.Sample(TextureCoord.xy).r;
    float alpha = smoothstep(weight - smoothing, weight + smoothing, distance);
    return float4(Color.rgb, Color.a * alpha);
}
