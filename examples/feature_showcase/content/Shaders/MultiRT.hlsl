cbuffer ModelConstantBuffer : register(b0) {
    matrix<float, 4, 4> Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    float4 Material;

    // {xyzw} = color.rgba
    float4 DiffuseColor;
};

cbuffer WorldConstantBuffer : register(b1) {
    matrix<float, 4, 4> ViewProjection;
    matrix<float, 4, 4> View;
    matrix<float, 4, 4> Projection;
    matrix<float, 4, 4> InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    float4 LightDirection;
};

struct VertexShaderInput {
    // {xyz_} = position.xyz
    float3 Position : SV_Position;
    float3 Normal : NORMAL0;
    float2 TextureCoord : TEXCOORD0;
};

struct VertexShaderOutput {
    float4 Position : SV_Position;
    float3 WorldSpacePosition : COLOR1;
    float3 Normal : NORMAL0;
    float2 TextureCoord : TEXCOORD0;
    float2 DepthZW :COLOR2;
};

float LambertianDiffuse(float3 lightDirection, float3 surfaceNormal)
{
    // NOTE: 'lightDirection' and 'surfaceNormal' have already been normalized.
    return max(dot(lightDirection, surfaceNormal), 0.0f);
}

float ComputePhongSpecular(
    float3 viewDirection,
    float3 lightDirection,
    float3 worldSpaceNormal,
    float specular)
{
    float3 halfVector = normalize(lightDirection.xyz + viewDirection);
    float specularAngle = max(0.0f, dot(halfVector, worldSpaceNormal));
    float density = 50.0f;
    return pow(specularAngle, density) * specular;
}

float ComputeLight(
    float3 viewDirection,
    float3 inverseLightDirection,
    float3 surfaceNormal,
    float specular)
{
    float kd = LambertianDiffuse(inverseLightDirection, surfaceNormal);

    // NOTE: Half-lambert
    kd = kd * 0.5f + 0.5f;
    kd = kd * kd;

    float ks = ComputePhongSpecular(
        viewDirection,
        inverseLightDirection,
        surfaceNormal,
        1.0f);

    return kd + ks;
}

float3 EncodeNormal(float3 normal)
{
    return normalize(normal) * 0.5f + 0.5f;
}

VertexShaderOutput MultiRTVS(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;

    float4 worldSpacePosition = mul(Model, float4(input.Position, 1.0f));

    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = Model[0].xyz;
    model3x3[1].xyz = Model[1].xyz;
    model3x3[2].xyz = Model[2].xyz;

    float4 position = mul(ViewProjection, worldSpacePosition);

    output.Position = position;
    output.WorldSpacePosition = worldSpacePosition.xyz;
    output.Normal = mul(model3x3, input.Normal.xyz);
    output.TextureCoord = input.TextureCoord.xy;
    output.DepthZW = position.zw;
    return output;
}

Texture2D<float4> diffuseTexture : register(t0);
SamplerState textureSampler : register(s0);

struct GBufferData {
    // {xyz_} = Diffuse Albedo RGB (R8G8B8)
    // {___w} = Alpha (A8)
    float4 Albedo : SV_Target0;

    // {xyz_} = World-space normal XYZ (R10G10B10)
    // {___w} = Unused (A2)
    float4 Normal : SV_Target1;

    // {x___} = Depth represented by the formula "z/w" (FP32)
    // {_yzw} = Unused
    float4 Depth : SV_Target2;

    // {xyz_} = Lighting Result RGB (R8G8B8)
    // {___w} = Alpha (A8)
    float4 Lighting : SV_Target3;
};

GBufferData MultiRTPS(VertexShaderOutput input)
{
    float4 albedoColor = diffuseTexture.Sample(textureSampler, input.TextureCoord.xy);
    albedoColor = albedoColor * DiffuseColor;

    float3 worldSpaceCameraPosition = (mul(InverseView, float4(0.0f, 0.0f, 0.0f, 1.0f))).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - input.WorldSpacePosition);
    float metalness = Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -LightDirection.xyz,
        normalize(input.Normal),
        metalness);

    float3 encodedNormal = EncodeNormal(input.Normal);

    GBufferData output;
    output.Albedo = albedoColor;
    output.Normal = float4(encodedNormal, 1.0f);
    output.Depth = float4(input.DepthZW.x / input.DepthZW.y, 0.0f, 0.0f, 1.0f);
    output.Lighting = float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
    return output;
}
