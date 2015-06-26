struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer ImageEffectConstants: register(b0) {
    vector<float, 2> RenderTargetPixelSize;
    vector<float, 2> RcpFrame;
};

Texture2D<float4> DiffuseTexture: register(t0);
SamplerState      TextureSampler: register(s0);

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
    float r = DiffuseTexture.Sample(TextureSampler, texCoord + red * amount).r;
    float g = DiffuseTexture.Sample(TextureSampler, texCoord + green * amount).g;
    float b = DiffuseTexture.Sample(TextureSampler, texCoord + blue * amount).b;
    return float3(r, g, b);
}

float4 ChromaticAberrationPS(VS_OUTPUT input) : SV_Target
{
    float2 inverseScreenSize = RcpFrame;
    float2 red = float2(-3.0, -3.0) * inverseScreenSize;
    float2 green = float2(0.0, 0.0) * inverseScreenSize;
    float2 blue = float2(3.0, 3.0) * inverseScreenSize;

    float3 color = ChromaticAberration(input.TextureCoord.xy, red, green, blue);
    return float4(color.rgb, 1.0);
}
