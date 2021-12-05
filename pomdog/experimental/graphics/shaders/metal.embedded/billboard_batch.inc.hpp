// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_BillboardBatch = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
constant int PomdogVertexBufferSlotOffset=20;
struct VertexInput{
float4 PositionTextureCoord [[attribute(0)]];};
struct InstanceVertex{
float4 Translation [[attribute(1)]];
float4 TextureCoordRect [[attribute(2)]];
float4 OriginPivotScale [[attribute(3)]];
float4 Color [[attribute(4)]];};
struct VertexOutput{
float4 Position [[position]];
float4 Color;
float2 TextureCoord;};
struct __attribute__((__aligned__(256)))WorldConstants{
matrix_float4x4 ViewProjection;
matrix_float4x4 View;
matrix_float4x4 Projection;
matrix_float4x4 InverseView;
float4 LightDirection;};
vertex VertexOutput BillboardBatchVS(
VertexInput input [[stage_in]],
constant InstanceVertex*instanceVertices [[buffer(1+PomdogVertexBufferSlotOffset)]],
constant WorldConstants& uniforms [[buffer(0)]],
ushort vertexIndex [[vertex_id]],
ushort instanceIndex [[instance_id]]){
constant InstanceVertex& perInstance=instanceVertices[instanceIndex];
float2 scale=perInstance.OriginPivotScale.zw;
float4x4 scaling=float4x4(
float4(scale.x,0.0,0.0,0.0),
float4(0.0,scale.y,0.0,0.0),
float4(0.0,0.0,1.0,0.0),
float4(0.0,0.0,0.0,1.0));
float rotationZ=perInstance.Translation.w;
float cosRotation=cos(rotationZ);
float sinRotation=sin(rotationZ);
float4x4 rotate=float4x4(
float4(cosRotation,sinRotation,0.0,0.0),
float4(-sinRotation,cosRotation,0.0,0.0),
float4(0.0,0.0,1.0,0.0),
float4(0.0,0.0,0.0,1.0));
float4x4 transform=rotate*scaling;
float4 worldSpacePosition=transform*float4(input.PositionTextureCoord.xy-perInstance.OriginPivotScale.xy,0.0,1.0);
float4 viewSpaceOffset=uniforms.View*float4(perInstance.Translation.xyz,1.0);
VertexOutput output;
output.Position=uniforms.Projection*float4(worldSpacePosition.xyz+viewSpaceOffset.xyz,1.0);
output.TextureCoord=input.PositionTextureCoord.zw*perInstance.TextureCoordRect.zw+perInstance.TextureCoordRect.xy;
output.Color=perInstance.Color;
return output;}
fragment half4 BillboardBatchPS(
VertexOutput input [[stage_in]],
texture2d<float>diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
return half4(color*input.Color);}
)";
