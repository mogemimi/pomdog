struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float4 Color        : COLOR0;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer DistanceFieldConstants : register(b2) {
    float Smoothing;
    float Weight;
};

Texture2D<float4> AlbedoTexture  : register(t0);
SamplerState      TextureSampler : register(s0);

float4 SpriteDistanceFieldPS(VS_OUTPUT input) : SV_Target
{
    float distance = AlbedoTexture.Sample(TextureSampler, input.TextureCoord.xy).a;
    float alpha = smoothstep(Weight - Smoothing, Weight + Smoothing, distance);
    return (input.Color.rgb, input.Color.a * alpha);
}
