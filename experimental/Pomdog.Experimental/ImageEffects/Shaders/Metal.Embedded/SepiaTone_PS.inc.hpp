// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_SepiaTone_PS = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VS_OUTPUT{
float4 Position [[position]];
float2 TextureCoord;};
namespace{
float3 ComputeSepiaTone(float3 color){
const float3 r=float3(0.393,0.769,0.189);
const float3 g=float3(0.349,0.686,0.168);
const float3 b=float3(0.272,0.534,0.131);
return clamp(float3(dot(color,r),dot(color,g),dot(color,b)),0.0,1.0);}}fragment half4 SepiaTonePS(
VS_OUTPUT input [[stage_in]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
color.rgb=ComputeSepiaTone(color.rgb);
return color;}
)";
