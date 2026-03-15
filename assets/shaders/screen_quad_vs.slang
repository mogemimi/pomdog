// Copyright mogemimi. Distributed under the MIT license.

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float2 TextureCoord;
};

[shader("vertex")]
float4 screen_quad_vs(
    VSInput input,
    [[vk::location(0)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    TextureCoord = input.TextureCoord.xy;
    return float4(input.Position.xyz, 1.0);
}
