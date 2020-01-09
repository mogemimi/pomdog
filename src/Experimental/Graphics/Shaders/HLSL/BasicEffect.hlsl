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

#if LIGHTING_ENABLED
    float3 Normal : NORMAL0;
#endif
#if TEXTURE_ENABLED
    float2 TextureCoord : TEXCOORD0;
#endif
#if VERTEX_COLOR_ENABLED
    float4 Color : COLOR0;
#endif
};

struct VertexShaderOutput {
    float4 Position : SV_Position;
#if LIGHTING_ENABLED
    float3 WorldSpacePosition : COLOR1;
    float3 Normal : NORMAL0;
#endif
#if TEXTURE_ENABLED
    float2 TextureCoord : TEXCOORD0;
#endif
#if VERTEX_COLOR_ENABLED
    float4 Color : COLOR0;
#endif
};

#if LIGHTING_ENABLED
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
        1.0);

    return kd + ks;
}
#endif

VertexShaderOutput BasicEffectVS(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;

    float4 worldSpacePosition = mul(Model, float4(input.Position, 1.0f));

#if LIGHTING_ENABLED
    // NOTE: Convert float4x4 to float3x3
    float3x3 model3x3;
    model3x3[0].xyz = Model[0].xyz;
    model3x3[1].xyz = Model[1].xyz;
    model3x3[2].xyz = Model[2].xyz;
#endif

    output.Position = mul(ViewProjection, worldSpacePosition);
#if LIGHTING_ENABLED
    output.WorldSpacePosition = worldSpacePosition.xyz;
    output.Normal = mul(model3x3, input.Normal.xyz);
#endif
#if TEXTURE_ENABLED
    output.TextureCoord = input.TextureCoord.xy;
#endif
#if VERTEX_COLOR_ENABLED
    output.Color = input.Color.rgba;
#endif
    return output;
}

#if TEXTURE_ENABLED
Texture2D<float4> diffuseTexture : register(t0);
SamplerState textureSampler : register(s0);
#endif

float4 BasicEffectPS(VertexShaderOutput input) : SV_Target
{
#if TEXTURE_ENABLED
    float4 albedoColor = diffuseTexture.Sample(textureSampler, input.TextureCoord.xy);
#elif VERTEX_COLOR_ENABLED
    float4 albedoColor = input.Color.rgba;
#else
    float4 albedoColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
#endif

#if LIGHTING_ENABLED
    float3 worldSpaceCameraPosition = (mul(InverseView, float4(0.0f, 0.0f, 0.0f, 1.0f))).xyz;
    float3 viewDirection = normalize(worldSpaceCameraPosition - input.WorldSpacePosition);
    float metalness = Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -LightDirection.xyz,
        normalize(input.Normal),
        metalness);

    return float4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
#else
    return albedoColor;
#endif
}
