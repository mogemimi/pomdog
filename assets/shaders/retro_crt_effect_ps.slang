// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D AlbedoSampler;

float3 RetroCrtEffect(float2 texCoord, float2 resolution)
{
    const float blendFactor = 0.1; // range (0.0, 1.0]
    float3 rgb = AlbedoSampler.Sample(texCoord).rgb;
    float artifact = cos(texCoord.y * resolution.y * 2.0) * 0.5 + 0.5;
    float x = artifact * blendFactor;
    return max(rgb - float3(x, x, x), float3(0.0, 0.0, 0.0));
}

[shader("pixel")]
float4 retro_crt_effect_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float3 color = RetroCrtEffect(TextureCoord.xy, cb.RenderTargetPixelSize);
    return float4(color.rgb, 1.0);
}
