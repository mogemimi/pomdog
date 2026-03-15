struct MyShaderConstants {
    float4x4 Model;
    float4x4 ViewProjection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<MyShaderConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D DiffuseTexture;

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float2 TextureCoord;
};

[shader("vertex")]
float4 simple_effect_vs(
    VSInput input,
    [[vk::location(0)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    float4 worldSpacePosition = mul(cb.Model, float4(input.Position.xyz, 1.0));
    TextureCoord = input.TextureCoord.xy;
    return mul(cb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 simple_effect_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 color = DiffuseTexture.Sample(TextureCoord.xy);
    return color;
}
