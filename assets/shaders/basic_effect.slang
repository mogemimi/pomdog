// Copyright mogemimi. Distributed under the MIT license.

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

// --- Lighting ---

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

float4 applyLighting(float4 albedoColor, float3 worldSpacePosition, float3 normal)
{
    float3 worldSpaceCameraPosition = mul(worldCb.InverseView, float4(0.0, 0.0, 0.0, 1.0)).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - worldSpacePosition);
    float metalness = modelCb.Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -worldCb.LightDirection.xyz,
        normalize(normal),
        metalness);

    return float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
}

// --- Variant: Position + Color ---

struct VSInput_PositionColor {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float4 Color;
};

[shader("vertex")]
float4 basic_effect_position_color_vs(
    VSInput_PositionColor input,
    [[vk::location(0)]] out float4 Color : COLOR0
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));
    Color = input.Color.rgba;
    return mul(worldCb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 basic_effect_position_color_ps(
    float4 sv_position : SV_Position,
    [[vk::location(0)]] float4 Color : COLOR0
) : SV_Target0
{
    return Color.rgba * modelCb.DiffuseColor;
}

// --- Variant: Position + Texture ---

struct VSInput_PositionTexture {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float2 TextureCoord;
};

[shader("vertex")]
float4 basic_effect_position_texture_vs(
    VSInput_PositionTexture input,
    [[vk::location(0)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));
    TextureCoord = input.TextureCoord.xy;
    return mul(worldCb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 basic_effect_position_texture_ps(
    float4 sv_position : SV_Position,
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 albedoColor = diffuseTexture.Sample(TextureCoord.xy);
    return albedoColor * modelCb.DiffuseColor;
}

// --- Variant: Position + Normal + Color ---

struct VSInput_PositionNormalColor {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float3 Normal;
    [[vk::location(2)]] float4 Color;
};

[shader("vertex")]
float4 basic_effect_position_normal_color_vs(
    VSInput_PositionNormalColor input,
    [[vk::location(0)]] out float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] out float3 Normal : NORMAL0,
    [[vk::location(2)]] out float4 Color : COLOR0
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));

    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = modelCb.Model[0].xyz;
    model3x3[1].xyz = modelCb.Model[1].xyz;
    model3x3[2].xyz = modelCb.Model[2].xyz;

    WorldSpacePosition = worldSpacePosition.xyz;
    Normal = mul(model3x3, input.Normal.xyz);
    Color = input.Color.rgba;

    return mul(worldCb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 basic_effect_position_normal_color_ps(
    float4 sv_position : SV_Position,
    [[vk::location(0)]] float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] float3 Normal : NORMAL0,
    [[vk::location(2)]] float4 Color : COLOR0
) : SV_Target0
{
    float4 albedoColor = Color.rgba * modelCb.DiffuseColor;
    return applyLighting(albedoColor, WorldSpacePosition, Normal);
}

// --- Variant: Position + Normal + Texture ---

struct VSInput_PositionNormalTexture {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float3 Normal;
    [[vk::location(2)]] float2 TextureCoord;
};

[shader("vertex")]
float4 basic_effect_position_normal_texture_vs(
    VSInput_PositionNormalTexture input,
    [[vk::location(0)]] out float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] out float3 Normal : NORMAL0,
    [[vk::location(2)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    float4 worldSpacePosition = mul(modelCb.Model, float4(input.Position.xyz, 1.0));

    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = modelCb.Model[0].xyz;
    model3x3[1].xyz = modelCb.Model[1].xyz;
    model3x3[2].xyz = modelCb.Model[2].xyz;

    WorldSpacePosition = worldSpacePosition.xyz;
    Normal = mul(model3x3, input.Normal.xyz);
    TextureCoord = input.TextureCoord.xy;

    return mul(worldCb.ViewProjection, worldSpacePosition);
}

[shader("pixel")]
float4 basic_effect_position_normal_texture_ps(
    float4 sv_position : SV_Position,
    [[vk::location(0)]] float3 WorldSpacePosition : COLOR1,
    [[vk::location(1)]] float3 Normal : NORMAL0,
    [[vk::location(2)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 albedoColor = diffuseTexture.Sample(TextureCoord.xy);
    albedoColor = albedoColor * modelCb.DiffuseColor;
    return applyLighting(albedoColor, WorldSpacePosition, Normal);
}
