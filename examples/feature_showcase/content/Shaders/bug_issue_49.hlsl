cbuffer MyShaderConstants : register(b0) {
    matrix<float, 4, 4> ViewProjection;
};

struct VertexShaderInput {
    // {xyz_} = position.xyz
    float3 Position : SV_Position;

    // {xy__} = texCoord.xy
    float2 TextureCoord : TEXCOORD;

    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation : COLOR0;

    // {xyzw} = color.rgba
    float4 Color : COLOR1;
};

struct VertexShaderOutput {
    float4 Position : SV_Position;
    float2 TextureCoord : TEXCOORD0;
    float4 Color : COLOR;
};

VertexShaderOutput BugIssue49_VS(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;

    float4x4 scaling = float4x4(
        float4(input.Translation.z, 0.0, 0.0, 0.0),
        float4(0.0, input.Translation.w, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float4x4 translate = float4x4(
        float4(1.0, 0.0, 0.0, 0.0),
        float4(0.0, 1.0, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(input.Translation.x, input.Translation.y, 0.0, 1.0));

    float4x4 transform = mul(scaling, translate);

    float4 worldSpacePosition = mul(float4(input.Position.xyz, 1.0), transform);
    output.Position = mul(worldSpacePosition, ViewProjection);
    output.TextureCoord = input.TextureCoord.xy;
    output.Color = input.Color;
    return output;
}

Texture2D<float4> DiffuseTexture : register(t2); // NOTE: not t0 to reproduce the issue
SamplerState      TextureSampler : register(s2);

float4 BugIssue49_PS(VertexShaderOutput input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);
    return input.Color * color;
}
