// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_LineBatch_PS = R"(
#version 330
in VertexData{
vec4 DestinationColor;}In;
out vec4 FragColor;
void main(){
FragColor=In.DestinationColor;}
)";
