// Copyright mogemimi. Distributed under the MIT license.

struct TransformMatrix {
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<TransformMatrix> cb;

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float4 Color;
};

[shader("vertex")]
float4 line_batch_vs(
    VSInput input,
    [[vk::location(0)]] out float4 DestinationColor : COLOR0
) : SV_Position
{
    DestinationColor = input.Color;
    // NOTE: 'ViewProjection' has already been transposed.
    return mul(float4(input.Position.xyz, 1.0), cb.ViewProjection);
}

[shader("pixel")]
float4 line_batch_ps(
    [[vk::location(0)]] float4 DestinationColor : COLOR0
) : SV_Target0
{
    return DestinationColor;
}
