// Copyright mogemimi. Distributed under the MIT license.

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

[shader("pixel")]
float4 sprite_batch_distance_field_outline_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(2)]] float4 OutlineColor : COLOR1,
    [[vk::location(3)]] float OutlineWeight : TEXCOORD2,
    [[vk::location(4)]] float Smoothing : TEXCOORD3,
    [[vk::location(5)]] float Weight : TEXCOORD4
) : SV_Target0
{
    float smoothing = Smoothing;
    float weight = Weight;
    float outlineWeight = OutlineWeight;

    // NOTE: Use R8 texture (not A8 texture)
    float distance = in_Texture.Sample(TextureCoord.xy).r;

    float outlineFactor = smoothstep(outlineWeight - smoothing, outlineWeight + smoothing, distance);
    float opacity = smoothstep(weight - smoothing, weight + smoothing, distance);

    float4 color = lerp(OutlineColor, Color, outlineFactor);

    return float4(color.rgb, color.a * opacity);
}
