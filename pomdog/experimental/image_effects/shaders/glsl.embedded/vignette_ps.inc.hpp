// Copyright mogemimi. Distributed under the MIT license.

constexpr auto Builtin_GLSL_Vignette_PS = R"(
#version 330
in QuadVertexShaderOutput{
vec2 TextureCoord;}In;
uniform ImageEffectConstants{
vec2 RenderTargetPixelSize;
vec2 RcpFrame;};
uniform VignetteBlock{
float Intensity;};
uniform sampler2D Texture;
out vec4 FragColor;
float SquaredVignette(vec2 textureCoord,float radius,float softness){
vec2 position=textureCoord-vec2(0.5);
vec2 squaredPos=position*position;
float len=sqrt(length(squaredPos));
return smoothstep(radius,radius-softness,len);}
void main(){
float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));
float radius=0.68-(0.05*Intensity);
float softness=0.01+(0.3*Intensity);
float vignette=SquaredVignette(In.TextureCoord.xy,radius,softness);
vec4 color=texture(Texture,In.TextureCoord.xy);
color.rgb*=vignette;
FragColor=dummy+vec4(color.rgb,1.0);}
)";
