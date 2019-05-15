struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

//cbuffer ImageEffectConstants: register(b0) {
//  vector<float, 2> RenderTargetPixelSize;
//  vector<float, 2> RcpFrame;
//};

cbuffer VignetteBlock: register(b1) {
    float Intensity;
};

Texture2D<float4> DiffuseTexture: register(t0);
SamplerState      TextureSampler: register(s0);

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

float4 VignettePS(VS_OUTPUT input) : SV_Target
{
    float radius = 0.68 - (0.05 * Intensity);
    float softness = 0.01 + (0.3 * Intensity);
    float vignette = SquaredVignette(input.TextureCoord.xy, radius, softness);
    float4 color = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);

    // Multiply blending
    color.rgb *= vignette;

    return color;
}
