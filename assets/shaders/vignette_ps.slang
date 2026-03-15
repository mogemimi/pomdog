// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

struct VignetteBlock {
    float Intensity;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> imageEffectCb;

[[vk::binding(1, 0)]]
ConstantBuffer<VignetteBlock> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
float SquaredVignette(float2 textureCoord, float radius, float softness)
{
    // NOTE: [0, 1] => [-0.5, +0.5]
    float2 position = textureCoord - float2(0.5, 0.5);
    float2 squaredPos = position * position;
    float len = sqrt(length(squaredPos));
    return smoothstep(radius, radius - softness, len);
}

[shader("pixel")]
float4 vignette_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float radius = 0.68 - (0.05 * cb.Intensity);
    float softness = 0.01 + (0.3 * cb.Intensity);
    float vignette = SquaredVignette(TextureCoord.xy, radius, softness);
    float4 color = in_Texture.Sample(TextureCoord.xy);

    // Multiply blending
    color.rgb *= vignette;

    return color;
}
