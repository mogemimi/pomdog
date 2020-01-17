// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SpriteBatchDistanceField_PS = R"(
#version 330
in VertexData{
vec4 Color;
vec4 BlendFactor;
vec2 TextureCoord;}In;
uniform sampler2D Texture;
out vec4 FragColor;
uniform SpriteBatchConstants{
mat4x4 ViewProjection;
vec4 DistanceFieldParameters;};
void main(){
float smoothing=DistanceFieldParameters.x;
float weight=DistanceFieldParameters.y;
float distance=texture(Texture,In.TextureCoord.xy).a;
float alpha=smoothstep(weight-smoothing,weight+smoothing,distance);
FragColor=vec4(In.Color.rgb,In.Color.a*alpha);}
)";
