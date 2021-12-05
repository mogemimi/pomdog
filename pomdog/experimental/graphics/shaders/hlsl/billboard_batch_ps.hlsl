struct VertexOutput {
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float2 TextureCoord : TEXCOORD0;
};

Texture2D<float4> diffuseTexture : register(t0);
SamplerState textureSampler : register(s0);

float4 BillboardBatchPS(VertexOutput input) : SV_Target
{
    float4 color = diffuseTexture.Sample(textureSampler, input.TextureCoord.xy);
    return color * input.Color;
}
