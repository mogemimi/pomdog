// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_ScreenQuad_VS = R"(
#version 330
layout(location=0)in vec3 Position;
layout(location=1)in vec2 TextureCoord;
out QuadVertexShaderOutput{
vec2 TextureCoord;}Out;
void main(){
gl_Position=vec4(Position.xyz,1.0);
Out.TextureCoord=TextureCoord.xy;}
)";
