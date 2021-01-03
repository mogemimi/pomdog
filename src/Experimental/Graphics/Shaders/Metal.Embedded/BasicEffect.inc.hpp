// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_BasicEffect = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct __attribute__((__aligned__(256)))ModelConstantBuffer{
matrix_float4x4 Model;
float4 Material;
float4 Color;};
struct __attribute__((__aligned__(256)))WorldConstantBuffer{
matrix_float4x4 ViewProjection;
matrix_float4x4 View;
matrix_float4x4 Projection;
matrix_float4x4 InverseView;
float4 LightDirection;};
struct VertexPositionColor{
float3 Position [[attribute(0)]];
float4 Color [[attribute(1)]];};
struct VertexPositionTexture{
float3 Position [[attribute(0)]];
float2 TextureCoord [[attribute(1)]];};
struct VertexPositionNormalColor{
float3 Position [[attribute(0)]];
float3 Normal [[attribute(1)]];
float4 Color [[attribute(2)]];};
struct VertexPositionNormalTexture{
float3 Position [[attribute(0)]];
float3 Normal [[attribute(1)]];
float2 TextureCoord [[attribute(2)]];};
struct VertexShaderOutput{
float4 Position [[position]];
#if LIGHTING_ENABLED
float3 WorldSpacePosition;
float3 Normal;
#endif
#if TEXTURE_ENABLED
float2 TextureCoord;
#endif
#if VERTEX_COLOR_ENABLED
float4 Color;
#endif
};
namespace{
#if LIGHTING_ENABLED
float LambertianDiffuse(float3 lightDirection,float3 surfaceNormal){
return max(dot(lightDirection,surfaceNormal),0.0);}
float ComputePhongSpecular(
float3 viewDirection,
float3 lightDirection,
float3 worldSpaceNormal,
float specular){
float3 halfVector=normalize(lightDirection.xyz+viewDirection);
float specularAngle=max(0.0,dot(halfVector,worldSpaceNormal));
const float density=50.0;
return pow(specularAngle,density)*specular;}
float ComputeLight(
float3 viewDirection,
float3 inverseLightDirection,
float3 surfaceNormal,
float specular){
float kd=LambertianDiffuse(inverseLightDirection,surfaceNormal);
kd=kd*0.5+0.5;
kd=kd*kd;
float ks=ComputePhongSpecular(
viewDirection,
inverseLightDirection,
surfaceNormal,
1.0);
return kd+ks;}
#endif
}
vertex VertexShaderOutput BasicEffectVS(
#if LIGHTING_ENABLED
#if TEXTURE_ENABLED
VertexPositionNormalTexture in [[stage_in]],
#else
VertexPositionNormalColor in [[stage_in]],
#endif
#else
#if TEXTURE_ENABLED
VertexPositionTexture in [[stage_in]],
#else
VertexPositionColor in [[stage_in]],
#endif
#endif
constant ModelConstantBuffer& modelConstants [[buffer(0)]],
constant WorldConstantBuffer& worldConstants [[buffer(1)]]){
VertexShaderOutput out;
float4 worldSpacePosition=modelConstants.Model*float4(in.Position,1.0);
#if LIGHTING_ENABLED
float3x3 model3x3;
model3x3[0].xyz=modelConstants.Model[0].xyz;
model3x3[1].xyz=modelConstants.Model[1].xyz;
model3x3[2].xyz=modelConstants.Model[2].xyz;
#endif
out.Position=worldConstants.ViewProjection*worldSpacePosition;
#if LIGHTING_ENABLED
out.WorldSpacePosition=worldSpacePosition.xyz;
out.Normal=model3x3*in.Normal.xyz;
#endif
#if TEXTURE_ENABLED
out.TextureCoord=in.TextureCoord.xy;
#endif
#if VERTEX_COLOR_ENABLED
out.Color=in.Color.rgba;
#endif
return out;}
fragment half4 BasicEffectPS(
VertexShaderOutput in [[stage_in]],
#if TEXTURE_ENABLED
texture2d<float>diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]],
#endif
constant ModelConstantBuffer& modelConstants [[buffer(0)]],
constant WorldConstantBuffer& worldConstants [[buffer(1)]]){
#if TEXTURE_ENABLED
float4 albedoColor=diffuseTexture.sample(textureSampler,in.TextureCoord);
#elif VERTEX_COLOR_ENABLED
float4 albedoColor=in.Color.rgba;
#else
float4 albedoColor=float4(1.0,1.0,1.0,1.0);
#endif
albedoColor=albedoColor*modelConstants.Color;
#if LIGHTING_ENABLED
float3 worldSpaceCameraPosition=(worldConstants.InverseView*float4(0.0,0.0,0.0,1.0)).xyz;
float3 viewDirection=normalize(worldSpaceCameraPosition-in.WorldSpacePosition);
float metalness=modelConstants.Material.x;
float directionalLightReflectance=ComputeLight(
viewDirection,
-worldConstants.LightDirection.xyz,
normalize(in.Normal),
metalness);
float4 color=float4(albedoColor.rgb*directionalLightReflectance,albedoColor.a);
return half4(color);
#else
return half4(albedoColor);
#endif
}
)";
