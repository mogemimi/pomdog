// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_Grayscale_PS = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VS_OUTPUT{
float4 Position [[position]];
float2 TextureCoord;};
namespace{
float ComputeLuma(float3 rgb){
const float3 luma=float3(0.299,0.587,0.114);
return dot(rgb,luma);}
float3 ComputeGrayscale(float3 color){
float luma=ComputeLuma(color);
return float3(luma,luma,luma);}}fragment float4 GrayscalePS(
VS_OUTPUT input [[stage_in]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
return float4(ComputeGrayscale(color.rgb),color.a);}
)";
