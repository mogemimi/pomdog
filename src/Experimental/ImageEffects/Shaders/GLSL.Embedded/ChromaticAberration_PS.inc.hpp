// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_ChromaticAberration_PS = R"(
#version 330
in QuadVertexShaderOutput{
vec2 TextureCoord;}In;
uniform ImageEffectConstants{
vec2 RenderTargetPixelSize;
vec2 RcpFrame;};
uniform sampler2D AlbedoSampler;
out vec4 FragColor;
float CircleVignette(in vec2 texCoord,in float radius,in float softness){
vec2 position=texCoord.xy-vec2(0.5);
float len=length(position);
float vignette=smoothstep(radius,radius-softness,len);
return vignette;}
vec3 ChromaticAberration(in vec2 texCoord,in vec2 red,in vec2 green,in vec2 blue){
const float radius=0.998;
const float softness=0.95;
float amount=1.0-CircleVignette(texCoord,radius,softness);
float r=texture(AlbedoSampler,texCoord+red*amount).r;
float g=texture(AlbedoSampler,texCoord+green*amount).g;
float b=texture(AlbedoSampler,texCoord+blue*amount).b;
return vec3(r,g,b);}
void main(){
vec2 inverseScreenSize=RcpFrame;
vec2 red=vec2(-3.0,-3.0)*inverseScreenSize;
vec2 green=vec2(0.0,0.0)*inverseScreenSize;
vec2 blue=vec2(3.0,3.0)*inverseScreenSize;
vec3 color=ChromaticAberration(In.TextureCoord.xy,red,green,blue);
FragColor=vec4(color.xyz,1.0);}
)";
