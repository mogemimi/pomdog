struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float4 Color        : COLOR0;
    float2 TextureCoord : TEXCOORD0;
};

Texture2D<float4> DiffuseTexture : register(t0);
SamplerState      TextureSampler : register(s0);

float4 SpriteBatchPS(VS_OUTPUT input): SV_Target
{
    float4 color = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);
    return color * input.Color;
}
