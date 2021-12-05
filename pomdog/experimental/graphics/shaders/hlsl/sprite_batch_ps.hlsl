struct VS_OUTPUT {
    float4 Position : SV_Position;
    float4 Color : COLOR0;

    // {x___} = RGB blend factor
    // {_y__} = Alpha blend factor
    // {__z_} = RGB compensation factor
    // {___w} = Alpha compensation factor
    float4 BlendFactor : COLOR1;

    float2 TextureCoord : TEXCOORD0;
};

Texture2D<float4> Texture : register(t0);
SamplerState      TextureSampler : register(s0);

float4 SpriteBatchPS(VS_OUTPUT input): SV_Target
{
    float4 color = Texture.Sample(TextureSampler, input.TextureCoord.xy);
    float4 blendFactor = float4(float3(1.0f, 1.0f, 1.0f) * input.BlendFactor.x, input.BlendFactor.y);
    float4 compensationFactor = float4(float3(1.0f, 1.0f, 1.0f) * input.BlendFactor.z, input.BlendFactor.w);
    color = min(color * blendFactor + compensationFactor, float4(1.0f, 1.0f, 1.0f, 1.0f));
    return color * input.Color;
}
