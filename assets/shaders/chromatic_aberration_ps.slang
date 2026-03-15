// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D AlbedoSampler;

float CircleVignette(float2 texCoord, float radius, float softness)
{
    // NOTE: [0, 1] => [-0.5, +0.5]
    float2 position = texCoord.xy - float2(0.5, 0.5);
    float len = length(position);
    float vignette = smoothstep(radius, radius - softness, len);
    return vignette;
}

float3 ChromaticAberration(float2 texCoord, float2 red, float2 green, float2 blue)
{
    const float radius = 0.998;
    const float softness = 0.95;

    float amount = 1.0 - CircleVignette(texCoord, radius, softness);
    float r = AlbedoSampler.Sample(texCoord + red * amount).r;
    float g = AlbedoSampler.Sample(texCoord + green * amount).g;
    float b = AlbedoSampler.Sample(texCoord + blue * amount).b;
    return float3(r, g, b);
}

[shader("pixel")]
float4 chromatic_aberration_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float2 inverseScreenSize = cb.RcpFrame;
    float2 red = float2(-3.0, -3.0) * inverseScreenSize;
    float2 green = float2(0.0, 0.0) * inverseScreenSize;
    float2 blue = float2(3.0, 3.0) * inverseScreenSize;

    float3 color = ChromaticAberration(TextureCoord.xy, red, green, blue);
    return float4(color.rgb, 1.0);
}
