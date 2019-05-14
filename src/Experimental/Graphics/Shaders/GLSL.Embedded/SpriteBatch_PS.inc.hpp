// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SpriteBatch_PS = R"(
#version 330
in VertexData{
vec4 Color;
vec2 TextureCoord;}In;
uniform sampler2D Texture;
out vec4 FragColor;
void main(){
vec4 color=texture(Texture,In.TextureCoord.xy);
FragColor=color*In.Color.xyzw;}
)";
