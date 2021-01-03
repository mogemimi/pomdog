// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_PolylineBatch = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
struct __attribute__((__aligned__(256)))TransformMatrix{
matrix_float4x4 ViewProjection;};
struct VS_INPUT{
float4 Position [[attribute(0)]];
float4 NextPointExtrusion [[attribute(1)]];
float4 PrevPointThickness [[attribute(2)]];
float4 Color [[attribute(3)]];};
struct VS_OUTPUT{
float4 Position [[position]];
float4 DestinationColor;};
vertex VS_OUTPUT PolylineBatchVS(
VS_INPUT input [[stage_in]],
constant TransformMatrix& uniforms [[buffer(0)]]){
float lineWidth=input.PrevPointThickness.w;
float extrusionDir=input.NextPointExtrusion.w;
float4 currentProjected=float4(input.Position.xyz,1.0)*uniforms.ViewProjection;
float4 nextProjected=float4(input.NextPointExtrusion.xyz,1.0)*uniforms.ViewProjection;
float4 prevProjected=float4(input.PrevPointThickness.xyz,1.0)*uniforms.ViewProjection;
float2 currentScreen=currentProjected.xy/currentProjected.w;
float2 nextScreen=nextProjected.xy/nextProjected.w;
float2 prevScreen=prevProjected.xy/prevProjected.w;
float2 toNext=normalize(nextScreen.xy-currentScreen.xy);
float2 normal=float2(-toNext.y,toNext.x);
float2 fromPrev=normalize(currentScreen.xy-prevScreen.xy);
float2 tangent=normalize(toNext+fromPrev);
float2 miter=float2(-tangent.y,tangent.x);
float thickness=lineWidth/dot(miter,normal);
float4 offset=float4(miter.xy*extrusionDir*thickness,0.0,0.0);
VS_OUTPUT out;
out.Position=currentProjected+offset;
out.DestinationColor=input.Color;
return out;}
fragment float4 PolylineBatchPS(VS_OUTPUT input [[stage_in]]){
return input.DestinationColor;}
)";
