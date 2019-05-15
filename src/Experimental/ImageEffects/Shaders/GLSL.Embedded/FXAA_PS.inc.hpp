// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_FXAA_PS = R"(
#version 330
float FxaaLuma(vec3 rgb){
const vec3 luma=vec3(0.299,0.587,0.114);
return dot(rgb,luma);}
vec4 FxaaPixelShader(sampler2D tex,vec4 posPos,vec2 rcpFrame){
vec3 rgbNW=textureLod(tex,posPos.zw,0.0).xyz;
vec3 rgbNE=textureLodOffset(tex,posPos.zw,0.0,ivec2(1,0)).xyz;
vec3 rgbSW=textureLodOffset(tex,posPos.zw,0.0,ivec2(0,1)).xyz;
vec3 rgbSE=textureLodOffset(tex,posPos.zw,0.0,ivec2(1,1)).xyz;
vec3 rgbM=textureLod(tex,posPos.xy,0.0).xyz;
float lumaNW=FxaaLuma(rgbNW);
float lumaNE=FxaaLuma(rgbNE);
float lumaSW=FxaaLuma(rgbSW);
float lumaSE=FxaaLuma(rgbSE);
float lumaM=FxaaLuma(rgbM);
float lumaMin=min(lumaM,min(min(lumaNW,lumaNE),min(lumaSW,lumaSE)));
float lumaMax=max(lumaM,max(max(lumaNW,lumaNE),max(lumaSW,lumaSE)));
float dirSwMinusNe=lumaSW-lumaNE;
float dirSeMinusNw=lumaSE-lumaNW;
vec2 dir=vec2(dirSwMinusNe+dirSeMinusNw,dirSwMinusNe-dirSeMinusNw);
const float FxaaReduceMin=1.0/128.0;
const float FxaaReduceMul=1.0/8.0;
const float FxaaSpanMax=8.0;
float dirReduce=max((lumaNW+lumaNE+lumaSW+lumaSE)*(0.25*FxaaReduceMul),
FxaaReduceMin);
float rcpDirMin=1.0/(min(abs(dir.x),abs(dir.y))+dirReduce);
dir=clamp(dir*rcpDirMin,vec2(-FxaaSpanMax),vec2(FxaaSpanMax))*rcpFrame.xy;
vec3 rgbN1=textureLod(tex,posPos.xy+dir*(1.0/3.0-0.5),0.0).xyz;
vec3 rgbP1=textureLod(tex,posPos.xy+dir*(2.0/3.0-0.5),0.0).xyz;
vec3 rgbN2=textureLod(tex,posPos.xy+dir*(0.0/3.0-0.5),0.0).xyz;
vec3 rgbP2=textureLod(tex,posPos.xy+dir*(3.0/3.0-0.5),0.0).xyz;
vec3 rgbA=(rgbN1+rgbP1);
vec3 rgbB=((rgbN2+rgbP2)*0.25)+(rgbA*0.25);
float lumaB=FxaaLuma(rgbB);
if((lumaB <lumaMin)||(lumaB>lumaMax)){
rgbB.xyz=rgbA.xyz*0.5;}
return vec4(rgbB,1.0);}
precision highp float;
in VertexData{
vec4 TextureCoord;}In;
uniform ImageEffectConstants{
vec2 RenderTargetPixelSize;
vec2 RcpFrame;};
uniform sampler2D Texture;
out vec4 FragColor;
void main(){
FragColor=FxaaPixelShader(Texture,In.TextureCoord,RcpFrame);}
)";
