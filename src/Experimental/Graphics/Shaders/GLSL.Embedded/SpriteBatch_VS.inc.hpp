// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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
vec2 TextureCoord;}Out;
uniform SpriteBatchConstants{
mat4x4 ViewProjection;};
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
mat3x3 transform =(scaling*rotate)* translate;
vec3 position=vec3(PositionTextureCoord.xy-OriginRotationDepth.xy,1.0)* transform;
vec4 finalPosition=vec4(position.xy,0.0,1.0)* ViewProjection;
gl_Position=vec4(finalPosition.xy,OriginRotationDepth.w,1.0);
Out.TextureCoord =(PositionTextureCoord.zw*SourceRect.zw+SourceRect.xy)* InverseTextureSize.xy;
Out.Color=Color;
}
)";
