// Copyright mogemimi. Distributed under the MIT license.

// NOTE: Texture is intentionally bound to slot 2 (not 0) to reproduce the issue.
//       This shader is used to reproduce the issue described in the following link:
//       https://github.com/mogemimi/pomdog/issues/49

struct MyShaderConstants {
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<MyShaderConstants> cb;

// NOTE: binding=2 (not 0) to reproduce the issue
[[vk::binding(2, 1)]]
Sampler2D DiffuseTexture;

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float2 TextureCoord;

    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy
    [[vk::location(2)]] float4 Translation;

    // {xyzw} = color.rgba
    [[vk::location(3)]] float4 Color;
};

[shader("vertex")]
float4 bug_issue_49_vs(
    VSInput input,
    [[vk::location(0)]] out float2 TextureCoord : TEXCOORD0,
    [[vk::location(1)]] out float4 Color : COLOR0
) : SV_Position
{
    float4x4 scaling = float4x4(
        float4(input.Translation.z, 0.0, 0.0, 0.0),
        float4(0.0, input.Translation.w, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float4x4 translate = float4x4(
        float4(1.0, 0.0, 0.0, 0.0),
        float4(0.0, 1.0, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(input.Translation.x, input.Translation.y, 0.0, 1.0));

    float4x4 transform = mul(scaling, translate);

    float4 worldSpacePosition = mul(float4(input.Position.xyz, 1.0), transform);
    TextureCoord = input.TextureCoord.xy;
    Color = input.Color;
    return mul(worldSpacePosition, cb.ViewProjection);
}

[shader("pixel")]
float4 bug_issue_49_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(1)]] float4 Color : COLOR0
) : SV_Target0
{
    float4 color = DiffuseTexture.Sample(TextureCoord.xy);
    return Color * color;
}
