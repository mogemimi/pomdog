// Copyright mogemimi. Distributed under the MIT license.

// NOTE: Multi Render Target Shader

struct ModelConstantBuffer {
    float4x4 Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    float4 Material;

    // {xyzw} = color.rgba
    float4 DiffuseColor;
};

struct WorldConstantBuffer {
    float4x4 ViewProjection;
    float4x4 View;
    float4x4 Projection;
    float4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    float4 LightDirection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ModelConstantBuffer> modelCb;

[[vk::binding(1, 0)]]
ConstantBuffer<WorldConstantBuffer> worldCb;

[[vk::binding(0, 1)]]
Sampler2D diffuseTexture;

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float3 Normal;
    [[vk::location(2)]] float2 TextureCoord;
};

float LambertianDiffuse(float3 lightDirection, float3 surfaceNormal)
{
    // NOTE: 'lightDirection' and 'surfaceNormal' have already been normalized.
    return max(dot(lightDirection, surfaceNormal), 0.0);
}

float ComputePhongSpecular(
    float3 viewDirection,
    float3 lightDirection,
    float3 worldSpaceNormal,
    float specular)
{
    float3 halfVector = normalize(lightDirection.xyz + viewDirection);
    float specularAngle = max(0.0, dot(halfVector, worldSpaceNormal));
    float density = 50.0;
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
    kd = kd * 0.5 + 0.5;
    kd = kd * kd;

    float ks = ComputePhongSpecular(
        viewDirection,
        inverseLightDirection,
        surfaceNormal,
        1.0);

    return kd + ks;
}

float3 EncodeNormal(float3 normal)
{
    return normalize(normal) * 0.5 + 0.5;
}

[shader("vertex")]
float4 multi_rt_vs(
    VSInput input,
    [[vk::location(0)]] out float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] out float3 Normal : NORMAL0,
    [[vk::location(2)]] out float2 TextureCoord : TEXCOORD0,
    [[vk::location(3)]] out float2 DepthZW : COLOR2
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));

    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = modelCb.Model[0].xyz;
    model3x3[1].xyz = modelCb.Model[1].xyz;
    model3x3[2].xyz = modelCb.Model[2].xyz;

    float4 position = mul(worldCb.ViewProjection, worldSpacePosition);

    WorldSpacePosition = worldSpacePosition.xyz;
    Normal = mul(model3x3, input.Normal.xyz);
    TextureCoord = input.TextureCoord.xy;
    DepthZW = position.zw;
    return position;
}

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

[shader("pixel")]
GBufferData multi_rt_ps(
    [[vk::location(0)]] float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] float3 Normal : NORMAL0,
    [[vk::location(2)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(3)]] float2 DepthZW : COLOR2
)
{
    float4 albedoColor = diffuseTexture.Sample(TextureCoord.xy);
    albedoColor = albedoColor * modelCb.DiffuseColor;

    float3 worldSpaceCameraPosition = mul(worldCb.InverseView, float4(0.0, 0.0, 0.0, 1.0)).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - WorldSpacePosition);
    float metalness = modelCb.Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -worldCb.LightDirection.xyz,
        normalize(Normal),
        metalness);

    float3 encodedNormal = EncodeNormal(Normal);

    GBufferData output;
    output.Albedo = albedoColor;
    output.Normal = float4(encodedNormal, 1.0);
    output.Depth = float4(DepthZW.x / DepthZW.y, 0.0, 0.0, 1.0);
    output.Lighting = float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
    return output;
}
