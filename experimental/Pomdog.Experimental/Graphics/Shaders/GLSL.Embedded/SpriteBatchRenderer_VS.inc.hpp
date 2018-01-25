// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SpriteBatchRenderer_VS = R"(
#version 330
layout(location=0)in vec4 PositionTextureCoord;
layout(location=1)in vec4 TransformMatrix1;
layout(location=2)in vec4 TransformMatrix2Origin;
layout(location=3)in vec4 SourceRect;
layout(location=4)in vec4 Color;
layout(location=5)in vec4 TextureSize;
out VertexData{
vec4 Color;
vec2 TextureCoord;}Out;
uniform SpriteBatchConstants{
mat4x4 ViewProjection;};
void main(){
mat2x2 scaling=mat2x2(
vec2(SourceRect.z,0.0),
vec2(0.0,SourceRect.w));
vec2 position=(PositionTextureCoord.xy-TransformMatrix2Origin.zw).xy*scaling;
mat3x2 worldMatrix=mat3x2(
vec2(TransformMatrix1.xy),
vec2(TransformMatrix1.zw),
vec2(TransformMatrix2Origin.xy));
position=(worldMatrix*vec3(position,1.0)).xy;
gl_Position=vec4(position.xy,0.0,1.0)*ViewProjection;
vec2 inverseTextureSize=TextureSize.xy;
Out.TextureCoord=(PositionTextureCoord.zw*SourceRect.zw+SourceRect.xy)*inverseTextureSize;
Out.Color=Color;}
)";
