// Copyright mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_PrimitiveBatch_PS = R"(
#version 330
in VertexData{
vec4 DestinationColor;}In;
out vec4 FragColor;
void main(){
FragColor=In.DestinationColor;}
)";
