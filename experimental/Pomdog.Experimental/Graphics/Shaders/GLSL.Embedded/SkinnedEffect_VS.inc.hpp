// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_SkinnedEffect_VS = R"(
#version 330
layout(location=0)in vec4 PositionTextureCoord;
layout(location=1)in vec4 Weights;
layout(location=2)in ivec4 JointIndices;
out VertexData{
vec2 TextureCoord;
vec4 Color;}Out;
uniform SkinningConstants{
vec4 Bones[192];};
uniform Constants{
mat4x4 ModelViewProjection;
vec4 MainColor;};
void main(){
mat3x2 skinning=mat3x2(0.0);
for(int i=0; i<4; ++i){
int jointIndex=JointIndices[i];
if(jointIndex<0){
break;}
mat3x2 boneMatrix=mat3x2(
vec2(Bones[jointIndex*2].xy),
vec2(Bones[jointIndex*2].zw),
vec2(Bones[jointIndex*2+1].xy));
skinning += boneMatrix*Weights[i];}
vec2 position=(skinning*vec3(PositionTextureCoord.xy,1.0)).xy;
gl_Position=vec4(position.xy,0.0,1.0)*ModelViewProjection;
Out.TextureCoord=PositionTextureCoord.zw;
Out.Color=MainColor;}
)";
