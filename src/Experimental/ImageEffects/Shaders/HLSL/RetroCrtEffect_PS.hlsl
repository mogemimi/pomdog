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

float3 RetroCrtEffect(float2 texCoord, float2 resolution)
{
    const float blendFactor = 0.1; // range (0.0, 1.0]
    float4 color = DiffuseTexture.Sample(TextureSampler, texCoord.xy);
    float artifact = cos(texCoord.y * resolution.y * 2.0) * 0.5 + 0.5;
    float x = artifact * blendFactor;
    return max(color.rgb - float3(x, x, x), float3(0, 0, 0));
}

float4 RetroCrtEffectPS(VS_OUTPUT input) : SV_Target
{
    float3 color = RetroCrtEffect(input.TextureCoord.xy, RenderTargetPixelSize);
    return float4(color.rgb, 1.0);
}
