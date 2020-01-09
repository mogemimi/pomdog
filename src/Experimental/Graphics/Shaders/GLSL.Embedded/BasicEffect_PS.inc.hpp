// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_BasicEffect_PS = R"(

uniform ModelConstantBuffer{
mat4x4 Model;
vec4 Material;
vec4 DiffuseColor;};
uniform WorldConstantBuffer{
mat4x4 ViewProjection;
mat4x4 View;
mat4x4 Projection;
mat4x4 InverseView;
vec4 LightDirection;};
in VertexData{
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
}In;
uniform sampler2D DiffuseTexture;
out vec4 FragColor;
#if LIGHTING_ENABLED
float LambertianDiffuse(vec3 lightDirection,vec3 surfaceNormal){
return max(dot(lightDirection,surfaceNormal),0.0);}
float ComputePhongSpecular(
vec3 viewDirection,
vec3 lightDirection,
vec3 worldSpaceNormal,
float specular){
vec3 halfVector=normalize(lightDirection.xyz+viewDirection);
float specularAngle=max(0.0,dot(halfVector,worldSpaceNormal));
float density=50.0;
return pow(specularAngle,density)*specular;}
float ComputeLight(
vec3 viewDirection,
vec3 inverseLightDirection,
vec3 surfaceNormal,
float specular){
float kd=LambertianDiffuse(inverseLightDirection,surfaceNormal);
kd=kd*0.5+0.5;
kd=kd*kd;
float ks=ComputePhongSpecular(
viewDirection,
inverseLightDirection,
surfaceNormal,
1.0);
return kd+ks;}
#endif
void main(){
#if TEXTURE_ENABLED
vec4 albedoColor=texture(DiffuseTexture,In.TextureCoord.xy);
#elif VERTEX_COLOR_ENABLED
vec4 albedoColor=In.Color.rgba;
#else
vec4 albedoColor=vec4(1.0,1.0,1.0,1.0);
#endif
#if LIGHTING_ENABLED
vec3 worldSpaceCameraPosition=(InverseView*vec4(0.0,0.0,0.0,1.0)).xyz;
vec3 viewDirection=normalize(worldSpaceCameraPosition-In.WorldSpacePosition);
float metalness=Material.x;
float directionalLightReflectance=ComputeLight(
viewDirection,
-LightDirection.xyz,
normalize(In.Normal),
metalness);
FragColor=vec4(albedoColor.rgb*directionalLightReflectance,albedoColor.a);
#else
FragColor=albedoColor;
#endif
}
)";
