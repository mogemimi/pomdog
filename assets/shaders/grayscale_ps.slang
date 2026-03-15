// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

float ComputeLuma(float3 rgb)
{
    const float3 luma = float3(0.299, 0.587, 0.114);
    return dot(rgb, luma);
}

float3 ComputeGrayscale(float3 color)
{
    float l = ComputeLuma(color);
    return float3(l, l, l);
}

[shader("pixel")]
float4 grayscale_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 color = in_Texture.Sample(TextureCoord.xy);
    return float4(ComputeGrayscale(color.rgb), color.a);
}
