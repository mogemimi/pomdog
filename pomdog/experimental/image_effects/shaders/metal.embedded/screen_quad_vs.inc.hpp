// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_ScreenQuad_VS = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct VS_INPUT{
float3 Position [[attribute(0)]];
float2 TextureCoord [[attribute(1)]];};
struct VS_OUTPUT{
float4 Position [[position]];
float2 TextureCoord;};
vertex VS_OUTPUT ScreenQuadVS(VS_INPUT input [[stage_in]]){
VS_OUTPUT output;
output.Position=float4(input.Position.xyz,1);
output.TextureCoord=input.TextureCoord.xy;
return output;}
)";
