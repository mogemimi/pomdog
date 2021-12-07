// Copyright mogemimi. Distributed under the MIT license.

constexpr auto Builtin_Metal_SpriteBatch = R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;
constant int PomdogVertexBufferSlotOffset=20;
struct VS_INPUT{
float4 PositionTextureCoord [[attribute(0)]];};
struct InstanceVertex{
float4 Translation [[attribute(1)]];
float4 SourceRect [[attribute(2)]];
float4 OriginRotationDepth [[attribute(3)]];
float4 Color [[attribute(4)]];
float4 InverseTextureSize [[attribute(5)]];};
struct VS_OUTPUT{
float4 Position [[position]];
float4 Color;
float4 BlendFactor;
float2 TextureCoord;};
struct __attribute__((__aligned__(256)))SpriteBatchConstants{
matrix_float4x4 ViewProjection;
float4 DistanceFieldParameters;};
vertex VS_OUTPUT SpriteBatchVS(
VS_INPUT input [[stage_in]],
constant InstanceVertex*instanceVertices [[buffer(1+PomdogVertexBufferSlotOffset)]],
constant SpriteBatchConstants& uniforms [[buffer(0)]],
ushort vertexIndex [[vertex_id]],
ushort instanceIndex [[instance_id]]){
constant InstanceVertex& perInstance=instanceVertices[instanceIndex];
matrix_float3x3 scaling=matrix_float3x3(
float3(perInstance.Translation.z*perInstance.SourceRect.z,0.0,0.0),
float3(0.0,perInstance.Translation.w*perInstance.SourceRect.w,0.0),
float3(0.0,0.0,1.0));
float cosRotation=cos(perInstance.OriginRotationDepth.z);
float sinRotation=sin(perInstance.OriginRotationDepth.z);
matrix_float3x3 rotate=matrix_float3x3(
float3(cosRotation,-sinRotation,0.0),
float3(sinRotation,cosRotation,0.0),
float3(0.0,0.0,1.0));
matrix_float3x3 translate=matrix_float3x3(
float3(1.0,0.0,perInstance.Translation.x),
float3(0.0,1.0,perInstance.Translation.y),
float3(0.0,0.0,1.0));
matrix_float3x3 transform=(scaling*rotate)*translate;
float3 position=float3(input.PositionTextureCoord.xy-perInstance.OriginRotationDepth.xy,1.0)*transform;
float4 finalPosition=float4(position.xy,0.0,1.0)*uniforms.ViewProjection;
float channelFlags=perInstance.InverseTextureSize.z;
bool sourceRGBEnabled=fmod(channelFlags,2)==1;
bool sourceAlphaEnabled=fmod(channelFlags,4)>=2;
bool compensationRGB=fmod(channelFlags,8)>=4;
bool compensationAlpha=fmod(channelFlags,16)>=8;
float4 blendFactor=float4(0.0,0.0,0.0,0.0);
if(sourceRGBEnabled){
blendFactor.x=1.0;}
if(sourceAlphaEnabled){
blendFactor.y=1.0;}
if(compensationRGB){
blendFactor.z=1.0;}
if(compensationAlpha){
blendFactor.w=1.0;}
VS_OUTPUT output;
output.Position=float4(finalPosition.xy,perInstance.OriginRotationDepth.w,1.0);
output.TextureCoord=((input.PositionTextureCoord.zw*perInstance.SourceRect.zw)
+perInstance.SourceRect.xy)*perInstance.InverseTextureSize.xy;
output.Color=perInstance.Color;
output.BlendFactor=blendFactor;
return output;}
fragment half4 SpriteBatchPS(
VS_OUTPUT input [[stage_in]],
texture2d<float>diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float4 color=diffuseTexture.sample(textureSampler,input.TextureCoord.xy);
float4 blendFactor=float4(float3(input.BlendFactor.x),input.BlendFactor.y);
float4 compensationFactor=float4(float3(input.BlendFactor.z),input.BlendFactor.w);
color=min(color*blendFactor+compensationFactor,float4(1.0));
return half4(color*input.Color);}
fragment half4 SpriteBatchDistanceFieldPS(
VS_OUTPUT input [[stage_in]],
constant SpriteBatchConstants& uniforms [[buffer(0)]],
texture2d<float>diffuseTexture [[texture(0)]],
sampler textureSampler [[sampler(0)]]){
float smoothing=uniforms.DistanceFieldParameters.x;
float weight=uniforms.DistanceFieldParameters.y;
float distance=diffuseTexture.sample(textureSampler,input.TextureCoord.xy).a;
float alpha=smoothstep(weight-smoothing,weight+smoothing,distance);
return half4(float4(input.Color.rgb,input.Color.a*alpha));}
)";
