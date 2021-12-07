// Copyright mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_BillboardBatch_PS = R"(
#version 330
in VertexData{
vec4 Color;
vec2 TextureCoord;}In;
uniform sampler2D DiffuseTexture;
out vec4 FragColor;
void main(){
vec4 color=texture(DiffuseTexture,In.TextureCoord.xy);
FragColor=color*In.Color;}
)";
