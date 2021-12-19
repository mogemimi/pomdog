#version 330

uniform ModelConstantBuffer {
    mat4x4 Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    vec4 Material;

    // {xyzw} = color.rgba
    vec4 DiffuseColor;
};

uniform WorldConstantBuffer {
    mat4x4 ViewProjection;
    mat4x4 View;
    mat4x4 Projection;
    mat4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    vec4 LightDirection;
};

in VertexData {
    vec3 WorldSpacePosition;
    vec3 Normal;
    vec2 TextureCoord;
    vec2 DepthZW;
} In;

float LambertianDiffuse(vec3 lightDirection, vec3 surfaceNormal)
{
    // NOTE: 'lightDirection' and 'surfaceNormal' have already been normalized.
    return max(dot(lightDirection, surfaceNormal), 0.0);
}

float ComputePhongSpecular(
    vec3 viewDirection,
    vec3 lightDirection,
    vec3 worldSpaceNormal,
    float specular)
{
    vec3 halfVector = normalize(lightDirection.xyz + viewDirection);
    float specularAngle = max(0.0, dot(halfVector, worldSpaceNormal));
    float density = 50.0;
    return pow(specularAngle, density) * specular;
}

float ComputeLight(
    vec3 viewDirection,
    vec3 inverseLightDirection,
    vec3 surfaceNormal,
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

vec3 EncodeNormal(vec3 normal)
{
    return normalize(normal) * 0.5 + 0.5;
}

uniform sampler2D DiffuseTexture;

// {xyz_} = Diffuse Albedo RGB (R8G8B8)
// {___w} = Alpha (A8)
layout(location = 0) out vec4 out_Albedo;

// {xyz_} = World-space normal XYZ (R10G10B10)
// {___w} = Unused (A2)
layout(location = 1) out vec4 out_Normal;

// {x___} = Depth represented by the formula "z/w" (FP32)
// {_yzw} = Unused
layout(location = 2) out float out_Depth;

// {xyz_} = Lighting Result RGB (R8G8B8)
// {___w} = Alpha (A8)
layout(location = 3) out vec4 out_Lighting;

void main()
{
    vec4 albedoColor = texture(DiffuseTexture, In.TextureCoord.xy);
    albedoColor = albedoColor * DiffuseColor;

    vec3 worldSpaceCameraPosition = (InverseView * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 viewDirection = normalize(worldSpaceCameraPosition - In.WorldSpacePosition);
    float metalness = Material.x;

    float directionalLightReflectance = ComputeLight(
        viewDirection,
        -LightDirection.xyz,
        normalize(In.Normal),
        metalness);

    vec3 encodedNormal = EncodeNormal(In.Normal);

    out_Albedo = vec4(albedoColor);
    out_Normal = vec4(encodedNormal, 1.0);
    out_Depth = In.DepthZW.x / In.DepthZW.y;
    out_Lighting = vec4(albedoColor.rgb * directionalLightReflectance, albedoColor.a);
}
