// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_Sprite_DistanceField_PS = R"(
#version 330
in VertexData{
vec2 TextureCoord;
vec4 Color;}In;
uniform sampler2D AlbedoTexture;
out vec4 FragColor;
uniform DistanceFieldConstants{
float Smoothing;
float Weight;};
void main(){
float distance=texture(AlbedoTexture,In.TextureCoord.xy).a;
float alpha=smoothstep(Weight-Smoothing,Weight+Smoothing,distance);
FragColor=vec4(In.Color.xyz,In.Color.w*alpha);}
)";
