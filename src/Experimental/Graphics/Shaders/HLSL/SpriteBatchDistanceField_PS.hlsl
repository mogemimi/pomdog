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

cbuffer SpriteBatchConstants : register(b0) {
    matrix<float, 4, 4> ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    float4 DistanceFieldParameters;
};

Texture2D<float4> Texture : register(t0);
SamplerState TextureSampler : register(s0);

float4 SpriteBatchDistanceFieldPS(VS_OUTPUT input) : SV_Target
{
    float smoothing = DistanceFieldParameters.x;
    float weight = DistanceFieldParameters.y;

    float distance = Texture.Sample(TextureSampler, input.TextureCoord.xy).a;
    float alpha = smoothstep(weight - smoothing, weight + smoothing, distance);
    return float4(input.Color.rgb, input.Color.a * alpha);
}
