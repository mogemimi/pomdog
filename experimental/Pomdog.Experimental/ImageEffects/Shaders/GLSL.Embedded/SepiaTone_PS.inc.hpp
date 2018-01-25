// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SepiaTone_PS = R"(
#version 330
in QuadVertexShaderOutput{
vec2 TextureCoord;}In;
uniform ImageEffectConstants{
vec2 RenderTargetPixelSize;
vec2 RcpFrame;};
uniform sampler2D Texture;
out vec4 FragColor;
vec3 ComputeSepiaTone(vec3 color){
const vec3 r=vec3(0.393,0.769,0.189);
const vec3 g=vec3(0.349,0.686,0.168);
const vec3 b=vec3(0.272,0.534,0.131);
return clamp(vec3(dot(color,r),dot(color,g),dot(color,b)),0.0,1.0);}
void main(){
vec4 color=texture(Texture,In.TextureCoord.xy);
color.rgb=ComputeSepiaTone(color.rgb);
float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));
FragColor=dummy+color;}
)";
