// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_BasicEffect_VS = R"(

layout(location=0)in vec3 Position;
#if LIGHTING_ENABLED
layout(location=1)in vec3 Normal;
#if TEXTURE_ENABLED
layout(location=2)in vec2 TextureCoord;
#else
layout(location=2)in vec4 Color;
#endif
#else
#if TEXTURE_ENABLED
layout(location=1)in vec2 TextureCoord;
#else
layout(location=1)in vec4 Color;
#endif
#endif
uniform ModelConstantBuffer{
mat4x4 Model;
vec4 Material;
vec4 DiffuseColor;};
uniform WorldConstantBuffer{
mat4x4 ViewProjection;
mat4x4 InverseView;
vec4 LightDirection;};
out VertexData{
#if LIGHTING_ENABLED
vec3 WorldSpacePosition;
vec3 Normal;
#endif
#if TEXTURE_ENABLED
vec2 TextureCoord;
#endif
#if VERTEX_COLOR_ENABLED
vec4 Color;
#endif
}Out;
void main(){
vec4 worldSpacePosition=Model*vec4(Position.xyz,1.0);
gl_Position=ViewProjection*worldSpacePosition;
#if LIGHTING_ENABLED
Out.WorldSpacePosition=worldSpacePosition.xyz;
Out.Normal=mat3x3(Model)*Normal.xyz;
#endif
#if TEXTURE_ENABLED
Out.TextureCoord=TextureCoord.xy;
#endif
#if VERTEX_COLOR_ENABLED
Out.Color=Color.rgba;
#endif
}
)";
