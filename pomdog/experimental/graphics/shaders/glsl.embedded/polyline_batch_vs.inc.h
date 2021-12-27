// Copyright mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_PolylineBatch_VS = R"(
#version 330
layout(location=0)in vec4 Position;
layout(location=1)in vec4 NextPointExtrusion;
layout(location=2)in vec4 PrevPointThickness;
layout(location=3)in vec4 Color;
out VertexData{
vec4 DestinationColor;}Out;
uniform TransformMatrix{
mat4 ViewProjection;};
void main(){
float lineWidth=PrevPointThickness.w;
float extrusionDir=NextPointExtrusion.w;
vec4 currentProjected=vec4(Position.xyz,1.0)*ViewProjection;
vec4 nextProjected=vec4(NextPointExtrusion.xyz,1.0)*ViewProjection;
vec4 prevProjected=vec4(PrevPointThickness.xyz,1.0)*ViewProjection;
vec2 currentScreen=currentProjected.xy/currentProjected.w;
vec2 nextScreen=nextProjected.xy/nextProjected.w;
vec2 prevScreen=prevProjected.xy/prevProjected.w;
vec2 toNext=normalize(nextScreen.xy-currentScreen.xy);
vec2 normal=vec2(-toNext.y,toNext.x);
vec2 fromPrev=normalize(currentScreen.xy-prevScreen.xy);
vec2 tangent=normalize(toNext+fromPrev);
vec2 miter=vec2(-tangent.y,tangent.x);
float thickness=lineWidth/dot(miter,normal);
vec4 offset=vec4(miter.xy*extrusionDir*thickness,0.0,0.0);
gl_Position=currentProjected+offset;
Out.DestinationColor=Color;}
)";
