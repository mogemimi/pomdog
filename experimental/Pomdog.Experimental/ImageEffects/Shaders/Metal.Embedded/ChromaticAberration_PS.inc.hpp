// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_ChromaticAberration_PS = R"(
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
float CircleVignette(float2 texCoord,float radius,float softness){
float2 position=texCoord.xy-float2(0.5,0.5);
float len=length(position);
float vignette=smoothstep(radius,radius-softness,len);
return vignette;}
float3 ChromaticAberration(
float2 texCoord,
float2 red,
float2 green,
float2 blue,
texture2d<float> diffuseTexture,
sampler textureSampler){
const float radius=0.998;
const float softness=0.95;
float amount=1.0-CircleVignette(texCoord,radius,softness);
float r=diffuseTexture.sample(textureSampler,texCoord+red*amount).r;
float g=diffuseTexture.sample(textureSampler,texCoord+green*amount).g;
float b=diffuseTexture.sample(textureSampler,texCoord+blue*amount).b;
return float3(r,g,b);}}fragment float4 ChromaticAberrationPS(
VS_OUTPUT input [[stage_in]],
constant ImageEffectConstants& uniforms [[buffer(0)]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float2 inverseScreenSize=uniforms.RcpFrame;
float2 red=float2(-3.0,-3.0)*inverseScreenSize;
float2 green=float2(0.0,0.0)*inverseScreenSize;
float2 blue=float2(3.0,3.0)*inverseScreenSize;
float3 color=ChromaticAberration(
input.TextureCoord.xy,red,green,blue,diffuseTexture,textureSampler);
return float4(color.rgb,1.0);}
)";
