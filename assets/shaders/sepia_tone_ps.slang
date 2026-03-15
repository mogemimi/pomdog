// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

float3 ComputeSepiaTone(float3 color)
{
    const float3 r = float3(0.393, 0.769, 0.189);
    const float3 g = float3(0.349, 0.686, 0.168);
    const float3 b = float3(0.272, 0.534, 0.131);
    return clamp(float3(dot(color, r), dot(color, g), dot(color, b)), 0.0, 1.0);
}

[shader("pixel")]
float4 sepia_tone_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 color = in_Texture.Sample(TextureCoord.xy);
    color.rgb = ComputeSepiaTone(color.rgb);
    return color;
}
