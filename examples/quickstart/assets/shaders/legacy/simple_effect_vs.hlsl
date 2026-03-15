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

struct VertexShaderOutput {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

VertexShaderOutput SimpleEffectVS(VS_INPUT input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;
    float4 worldSpacePosition = mul(Model, float4(input.Position.xyz, 1.0));
    output.Position = mul(ViewProjection, worldSpacePosition);
    output.TextureCoord = input.TextureCoord.xy;
    return output;
}
