struct VS_INPUT {
    // {xyz_} = position.xyz
    float3 Position : SV_Position;

    // {xy__} = texCoord.xy
    float2 TextureCoord : TEXCOORD;
};

cbuffer MyShaderConstants : register(b0) {
    matrix<float, 4, 4> Model;
    matrix<float, 4, 4> ViewProjection;
};

struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

VS_OUTPUT SimpleEffectVS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(mul(float4(input.Position.xyz, 1.0), Model), ViewProjection);
    output.TextureCoord = input.TextureCoord.xy;
    return output;
}
