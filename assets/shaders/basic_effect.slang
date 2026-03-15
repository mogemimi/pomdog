// Copyright mogemimi. Distributed under the MIT license.

// Preprocessor defines:
//   LIGHTING_ENABLED      - Enable lighting calculations
//   TEXTURE_ENABLED       - Enable texture sampling
//   VERTEX_COLOR_ENABLED  - Enable per-vertex color

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

#if TEXTURE_ENABLED
[[vk::binding(0, 1)]]
Sampler2D diffuseTexture;
#endif

struct VSInput {
    [[vk::location(0)]] float3 Position;

#if LIGHTING_ENABLED
    [[vk::location(1)]] float3 Normal;
#if TEXTURE_ENABLED
    [[vk::location(2)]] float2 TextureCoord;
#else
    [[vk::location(2)]] float4 Color;
#endif
#else
#if TEXTURE_ENABLED
    [[vk::location(1)]] float2 TextureCoord;
#else
    [[vk::location(1)]] float4 Color;
#endif
#endif
};

#if LIGHTING_ENABLED
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
#endif

[shader("vertex")]
float4 basic_effect_vs(
    VSInput input
#if LIGHTING_ENABLED
    , [[vk::location(0)]] out float3 WorldSpacePosition : COLOR1
    , [[vk::location(1)]] out float3 Normal : NORMAL0
#endif
#if TEXTURE_ENABLED
    , [[vk::location(2)]] out float2 TextureCoord : TEXCOORD0
#endif
#if VERTEX_COLOR_ENABLED
    , [[vk::location(3)]] out float4 Color : COLOR0
#endif
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));

#if LIGHTING_ENABLED
    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = modelCb.Model[0].xyz;
    model3x3[1].xyz = modelCb.Model[1].xyz;
    model3x3[2].xyz = modelCb.Model[2].xyz;

    WorldSpacePosition = worldSpacePosition.xyz;
    Normal = mul(model3x3, input.Normal.xyz);
#endif
#if TEXTURE_ENABLED
    TextureCoord = input.TextureCoord.xy;
#endif
#if VERTEX_COLOR_ENABLED
    Color = input.Color.rgba;
#endif

    return mul(worldCb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 basic_effect_ps(
    float4 sv_position : SV_Position
#if LIGHTING_ENABLED
    , [[vk::location(0)]] float3 WorldSpacePosition : COLOR1
    , [[vk::location(1)]] float3 Normal : NORMAL0
#endif
#if TEXTURE_ENABLED
    , [[vk::location(2)]] float2 TextureCoord : TEXCOORD0
#endif
#if VERTEX_COLOR_ENABLED
    , [[vk::location(3)]] float4 Color : COLOR0
#endif
) : SV_Target0
{
#if TEXTURE_ENABLED
    float4 albedoColor = diffuseTexture.Sample(TextureCoord.xy);
#elif VERTEX_COLOR_ENABLED
    float4 albedoColor = Color.rgba;
#else
    float4 albedoColor = float4(1.0, 1.0, 1.0, 1.0);
#endif
    albedoColor = albedoColor * modelCb.DiffuseColor;

#if LIGHTING_ENABLED
    float3 worldSpaceCameraPosition = mul(worldCb.InverseView, float4(0.0, 0.0, 0.0, 1.0)).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - WorldSpacePosition);
    float metalness = modelCb.Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -worldCb.LightDirection.xyz,
        normalize(Normal),
        metalness);

    return float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
#else
    return albedoColor;
#endif
}
