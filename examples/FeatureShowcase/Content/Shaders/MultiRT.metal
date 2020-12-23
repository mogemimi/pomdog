#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct __attribute__((__aligned__(256))) ModelConstantBuffer {
    matrix_float4x4 Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    float4 Material;

    // {xyzw} = color.rgba
    float4 Color;
};

struct __attribute__((__aligned__(256))) WorldConstantBuffer {
    matrix_float4x4 ViewProjection;
    matrix_float4x4 View;
    matrix_float4x4 Projection;
    matrix_float4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    float4 LightDirection;
};

struct VertexPositionNormalTexture {
    float3 Position [[attribute(0)]];
    float3 Normal [[attribute(1)]];
    float2 TextureCoord [[attribute(2)]];
};

struct VertexShaderOutput {
    float4 Position [[position]];
    float3 WorldSpacePosition;
    float3 Normal;
    float2 TextureCoord;
    float2 DepthZW;
};

namespace {

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
    const float density = 50.0;
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

} // namespace

vertex VertexShaderOutput
MultiRTVS(
    VertexPositionNormalTexture in [[stage_in]],
    constant ModelConstantBuffer& modelConstants [[buffer(0)]],
    constant WorldConstantBuffer& worldConstants [[buffer(1)]])
{
    VertexShaderOutput out;

    float4 worldSpacePosition = modelConstants.Model * float4(in.Position, 1.0);

    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = modelConstants.Model[0].xyz;
    model3x3[1].xyz = modelConstants.Model[1].xyz;
    model3x3[2].xyz = modelConstants.Model[2].xyz;

    float4 position = worldConstants.ViewProjection * worldSpacePosition;

    out.Position = position;
    out.WorldSpacePosition = worldSpacePosition.xyz;
    out.Normal = model3x3 * in.Normal.xyz;
    out.TextureCoord = in.TextureCoord.xy;
    out.DepthZW = position.zw;
    return out;
}

struct GBufferData {
    // {xyz_} = Diffuse Albedo RGB (R8G8B8)
    // {___w} = Alpha (A8)
    half4 Albedo [[color(0)]];

    // {xyz_} = World-space normal XYZ (R10G10B10)
    // {___w} = Unused (A2)
    half4 Normal [[color(1)]];

    // {x___} = Depth represented by the formula "z/w" (FP32)
    // {_yzw} = Unused
    float Depth [[color(2)]];

    // {xyz_} = Lighting Result RGB (R8G8B8)
    // {___w} = Alpha (A8)
    half4 Lighting [[color(3)]];
};

fragment GBufferData
MultiRTPS(
    VertexShaderOutput in [[stage_in]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]],
    constant ModelConstantBuffer& modelConstants [[buffer(0)]],
    constant WorldConstantBuffer& worldConstants [[buffer(1)]])
{
    float4 albedoColor = diffuseTexture.sample(textureSampler, in.TextureCoord);
    albedoColor = albedoColor * modelConstants.Color;

    float3 worldSpaceCameraPosition = (worldConstants.InverseView * float4(0.0, 0.0, 0.0, 1.0)).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - in.WorldSpacePosition);
    float metalness = modelConstants.Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -worldConstants.LightDirection.xyz,
        normalize(in.Normal),
        metalness);

    float3 encodedNormal = EncodeNormal(in.Normal);

    GBufferData out;
    out.Albedo = half4(albedoColor);
    out.Normal = half4(half3(encodedNormal), 1.0);
    out.Depth = in.DepthZW.x / in.DepthZW.y;
    out.Lighting = half4(float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a));
    return out;
}
