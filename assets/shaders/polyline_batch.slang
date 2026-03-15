// Copyright mogemimi. Distributed under the MIT license.

struct TransformMatrix {
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<TransformMatrix> cb;

struct VSInput {
    // {xyz_} = position.xyz
    // {___w} = unused
    [[vk::location(0)]] float4 Position;

    // {xyz_} = nextPoint.xyz
    // {___w} = extrusion
    [[vk::location(1)]] float4 NextPointExtrusion;

    // {xyz_} = prevPoint.xyz
    // {___w} = lineThickness
    [[vk::location(2)]] float4 PrevPointThickness;

    // {xyzw} = color.rgba
    [[vk::location(3)]] float4 Color;
};

[shader("vertex")]
float4 polyline_batch_vs(
    VSInput input,
    [[vk::location(0)]] out float4 DestinationColor : COLOR0
) : SV_Position
{
    float lineWidth = input.PrevPointThickness.w;
    float extrusionDir = input.NextPointExtrusion.w;

    float4 currentProjected = mul(float4(input.Position.xyz, 1.0), cb.ViewProjection);
    float4 nextProjected = mul(float4(input.NextPointExtrusion.xyz, 1.0), cb.ViewProjection);
    float4 prevProjected = mul(float4(input.PrevPointThickness.xyz, 1.0), cb.ViewProjection);

    float2 currentScreen = currentProjected.xy / currentProjected.w;
    float2 nextScreen = nextProjected.xy / nextProjected.w;
    float2 prevScreen = prevProjected.xy / prevProjected.w;

    float2 toNext = normalize(nextScreen.xy - currentScreen.xy);
    float2 normal = float2(-toNext.y, toNext.x);

    float2 fromPrev = normalize(currentScreen.xy - prevScreen.xy);
    float2 tangent = normalize(toNext + fromPrev);
    float2 miter = float2(-tangent.y, tangent.x);

    float thickness = lineWidth / dot(miter, normal);
    float4 offset = float4(miter.xy * extrusionDir * thickness, 0.0, 0.0);

    DestinationColor = input.Color;
    return currentProjected + offset;
}

[shader("pixel")]
float4 polyline_batch_ps(
    [[vk::location(0)]] float4 DestinationColor : COLOR0
) : SV_Target0
{
    return DestinationColor;
}
