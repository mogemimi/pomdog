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

float ComputeLuma(float3 rgb)
{
    const float3 luma = float3(0.299, 0.587, 0.114);
    return dot(rgb, luma);
}

float3 ComputeGrayscale(float3 color)
{
    float luma = ComputeLuma(color);
    return float3(luma, luma, luma);
}

float4 GrayscalePS(VS_OUTPUT input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);
    return float4(ComputeGrayscale(color.rgb), color.a);
}
