// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_Vignette_PS = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VS_OUTPUT{
float4 Position [[position]];
float2 TextureCoord;};
struct __attribute__((__aligned__(256)))VignetteBlock{
float Intensity;};
namespace{
float SquaredVignette(float2 textureCoord,float radius,float softness){
float2 position=textureCoord-float2(0.5,0.5);
float2 squaredPos=position*position;
float len=sqrt(length(squaredPos));
return smoothstep(radius,radius-softness,len);}}fragment float4 VignettePS(
VS_OUTPUT input [[stage_in]],
constant VignetteBlock& vignetteUniforms [[buffer(1)]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float radius=0.68-(0.05*vignetteUniforms.Intensity);
float softness=0.01+(0.3*vignetteUniforms.Intensity);
float vignette=SquaredVignette(input.TextureCoord.xy,radius,softness);
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
color.rgb *= vignette;
return color;}
)";
