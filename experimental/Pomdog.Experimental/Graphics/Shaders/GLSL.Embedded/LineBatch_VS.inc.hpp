// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_LineBatch_VS = R"(
#version 330
layout(location=0)in vec3 Position;
layout(location=1)in vec4 Color;
out VertexData{
vec4 DestinationColor;}Out;
uniform TransformMatrix{
mat4 ViewProjection;};
void main(){
gl_Position=vec4(Position.xyz,1.0)*ViewProjection;
Out.DestinationColor=Color;}
)";
