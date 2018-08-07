// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_RetroCrtEffect_PS = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VS_OUTPUT{
float4 Position [[position]];
float2 TextureCoord;};
struct __attribute__((__aligned__(256)))ImageEffectConstants{
float2 RenderTargetPixelSize;
float2 RcpFrame;};
namespace{
float3 RetroCrtEffect(
float2 texCoord,
float2 resolution,
texture2d<float> diffuseTexture,
sampler textureSampler){
const float blendFactor=0.1; float4 color=diffuseTexture.sample(textureSampler,texCoord.xy);
float artifact=cos(texCoord.y*resolution.y*2.0)*0.5+0.5;
float x=artifact*blendFactor;
return max(color.rgb-float3(x,x,x),float3(0,0,0));}}fragment float4 RetroCrtEffectPS(
VS_OUTPUT input [[stage_in]],
constant ImageEffectConstants& uniforms [[buffer(0)]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float3 color=RetroCrtEffect(
input.TextureCoord.xy,
uniforms.RenderTargetPixelSize,
diffuseTexture,
textureSampler);
return float4(color.rgb,1.0);}
)";
