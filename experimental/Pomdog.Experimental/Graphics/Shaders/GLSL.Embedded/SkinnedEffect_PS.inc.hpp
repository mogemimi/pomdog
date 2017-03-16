// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SkinnedEffect_PS = R"(
#version 330
in VertexData{
vec2 TextureCoord;
vec4 Color;}In;
uniform sampler2D DiffuseTexture;
out vec4 FragColor;
void main(){
FragColor=texture(DiffuseTexture,In.TextureCoord.xy).xyzw*In.Color.xyzw;}
)";
