// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_RetroCrtEffect_PS = R"(
#version 330
in QuadVertexShaderOutput{
vec2 TextureCoord;}In;
uniform ImageEffectConstants{
vec2 RenderTargetPixelSize;
vec2 RcpFrame;};
uniform sampler2D AlbedoSampler;
out vec4 FragColor;
vec3 RetroCrtEffect(in vec2 texCoord,in vec2 resolution){
const float blendFactor=0.1; vec3 rgb=texture(AlbedoSampler,texCoord).rgb;
float artifact=cos(texCoord.y*resolution.y*2.0)*0.5+0.5;
return max(rgb-vec3(artifact*blendFactor),vec3(0.0));}
void main(){
vec3 color=RetroCrtEffect(In.TextureCoord.xy,RenderTargetPixelSize);
FragColor=vec4(color.xyz,1.0);}
)";
