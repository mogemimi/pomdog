struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float4 Color        : COLOR0;
    float2 TextureCoord : TEXCOORD0;
    uint TextureIndex   : BLENDINDICES0;
};

Texture2D<float4> DiffuseTexture0 : register(t0);
Texture2D<float4> DiffuseTexture1 : register(t1);
Texture2D<float4> DiffuseTexture2 : register(t2);
Texture2D<float4> DiffuseTexture3 : register(t3);
SamplerState      TextureSampler  : register(s0);

float4 FindTextureColor(uint textureIndex, float2 uv)
{
    if (textureIndex == 0) {
        return DiffuseTexture0.Sample(TextureSampler, uv);
    }
    else if (textureIndex == 1) {
        return DiffuseTexture1.Sample(TextureSampler, uv);
    }
    else if (textureIndex == 2) {
        return DiffuseTexture2.Sample(TextureSampler, uv);
    }
    return DiffuseTexture3.Sample(TextureSampler, uv);
}

float4 SpriteBatchRendererPS(VS_OUTPUT input) : SV_Target
{
    float4 color = FindTextureColor(input.TextureIndex, input.TextureCoord.xy);
    return color * input.Color;
}
