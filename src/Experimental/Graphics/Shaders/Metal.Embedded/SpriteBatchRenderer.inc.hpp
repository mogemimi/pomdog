// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_SpriteBatchRenderer = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
constant int PomdogVertexBufferSlotOffset=20;
struct __attribute__((__aligned__(256)))SpriteBatchConstants{
matrix_float4x4 ViewProjection;};
struct VS_INPUT{
float4 PositionTextureCoord [[attribute(0)]];};
struct InstanceVertex{
float4 TransformMatrix1 [[attribute(1)]];
float4 TransformMatrix2Origin [[attribute(2)]];
float4 SourceRect [[attribute(3)]];
float4 Color [[attribute(4)]];
float4 TextureSize [[attribute(5)]];};
struct VS_OUTPUT{
float4 Position [[position]];
float4 Color;
float2 TextureCoord;};
vertex VS_OUTPUT SpriteBatchRendererVS(
VS_INPUT                       input            [[stage_in]],
constant InstanceVertex*       instanceVertices [[buffer(1+PomdogVertexBufferSlotOffset)]],
constant SpriteBatchConstants& uniforms         [[buffer(0)]],
ushort                         vertexIndex      [[vertex_id]],
ushort                         instanceIndex    [[instance_id]]){
constant InstanceVertex& perInstance=instanceVertices[instanceIndex];
matrix_float2x2 scaling=matrix_float2x2(
float2(perInstance.SourceRect.z,0.0),
float2(0.0,perInstance.SourceRect.w));
float2 position=(input.PositionTextureCoord.xy-perInstance.TransformMatrix2Origin.zw).xy*scaling;
matrix_float3x2 worldMatrix=matrix_float3x2(
float2(perInstance.TransformMatrix1.xy),
float2(perInstance.TransformMatrix1.zw),
float2(perInstance.TransformMatrix2Origin.xy));
position=(worldMatrix*float3(position,1.0)).xy;
float2 inverseTextureSize=perInstance.TextureSize.xy;
VS_OUTPUT output;
output.Position=float4(position.xy,0.0,1.0)*uniforms.ViewProjection;
output.TextureCoord=(input.PositionTextureCoord.zw*perInstance.SourceRect.zw+perInstance.SourceRect.xy)*inverseTextureSize;
output.Color=perInstance.Color;
return output;}
fragment float4 SpriteBatchRendererPS(
VS_OUTPUT input [[stage_in]],
texture2d<float> diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
return color*input.Color;}
)";
