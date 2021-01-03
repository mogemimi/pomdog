// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SpriteBatch_VS = R"(
#version 330
layout(location=0)in vec4 PositionTextureCoord;
layout(location=1)in vec4 Translation;
layout(location=2)in vec4 SourceRect;
layout(location=3)in vec4 OriginRotationDepth;
layout(location=4)in vec4 Color;
layout(location=5)in vec4 InverseTextureSize;
out VertexData{
vec4 Color;
vec4 BlendFactor;
vec2 TextureCoord;}Out;
uniform SpriteBatchConstants{
mat4x4 ViewProjection;
vec4 DistanceFieldParameters;};
void main(){
mat3x3 scaling=mat3x3(
vec3(Translation.z*SourceRect.z,0.0,0.0),
vec3(0.0,Translation.w*SourceRect.w,0.0),
vec3(0.0,0.0,1.0));
float cosRotation=cos(OriginRotationDepth.z);
float sinRotation=sin(OriginRotationDepth.z);
mat3x3 rotate=mat3x3(
vec3(cosRotation,-sinRotation,0.0),
vec3(sinRotation,cosRotation,0.0),
vec3(0.0f,0.0f,1.0));
mat3x3 translate=mat3x3(
vec3(1.0,0.0,Translation.x),
vec3(0.0,1.0,Translation.y),
vec3(0.0,0.0,1.0));
mat3x3 transform=(scaling*rotate)*translate;
vec3 position=vec3(PositionTextureCoord.xy-OriginRotationDepth.xy,1.0)*transform;
vec4 finalPosition=vec4(position.xy,0.0,1.0)*ViewProjection;
float channelFlags=InverseTextureSize.z;
bool sourceRGBEnabled=mod(channelFlags,2)==1;
bool sourceAlphaEnabled=mod(channelFlags,4)>=2;
bool compensationRGB=mod(channelFlags,8)>=4;
bool compensationAlpha=mod(channelFlags,16)>=8;
vec4 blendFactor=vec4(0.0,0.0,0.0,0.0);
if(sourceRGBEnabled){
blendFactor.x=1.0;}
if(sourceAlphaEnabled){
blendFactor.y=1.0;}
if(compensationRGB){
blendFactor.z=1.0;}
if(compensationAlpha){
blendFactor.w=1.0;}
gl_Position=vec4(finalPosition.xy,OriginRotationDepth.w,1.0);
Out.TextureCoord=(PositionTextureCoord.zw*SourceRect.zw+SourceRect.xy)*InverseTextureSize.xy;
Out.Color=Color;
Out.BlendFactor=blendFactor;}
)";
