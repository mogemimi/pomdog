struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer FxaaPassBuffer: register(b0) {
    vector<float, 2> RcpFrame;
};

Texture2D<float4> DiffuseTexture: register(t0);
SamplerState      TextureSampler: register(s0);

float3 ComputeSepiaTone(float3 color)
{
    const float3 r = float3(0.393, 0.769, 0.189);
    const float3 g = float3(0.349, 0.686, 0.168);
    const float3 b = float3(0.272, 0.534, 0.131);
    return clamp(float3(dot(color, r), dot(color, g), dot(color, b)), 0.0, 1.0);
}

float4 SepiaTonePS(VS_OUTPUT input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);
    color.rgb = ComputeSepiaTone(color.rgb);
    return color;
}
