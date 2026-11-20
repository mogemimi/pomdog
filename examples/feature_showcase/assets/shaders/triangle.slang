// Copyright mogemimi. Distributed under the MIT license.

// NOTE: Triangle Shader

struct VSInput {
    // {xyz} = position in clip space
    [[vk::location(0)]] float3 Position;

    // {xyzw} = color.rgba
    [[vk::location(1)]] float4 Color;
};

[shader("vertex")]
float4 triangle_vs(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0
) : SV_Position
{
    Color = input.Color;
    return float4(input.Position.xyz, 1.0);
}

[shader("pixel")]
float4 triangle_ps(
    [[vk::location(0)]] float4 Color : COLOR0
) : SV_Target0
{
    return Color;
}
