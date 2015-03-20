// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

char const* Builtin_GLSL_Vignette_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;}In;\n"
"uniform Constants{\n"
"vec2 RenderTargetPixelSize;};\n"
"uniform VignetteBlock{\n"
"float Intensity;};\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"float SquaredVignette(vec2 textureCoord,float radius,float softness){\n"
"vec2 position=textureCoord-vec2(0.5);\n"
"vec2 squaredPos=position*position;\n"
"float len=sqrt(length(squaredPos));\n"
"float vignette=smoothstep(radius,radius-softness,len);\n"
"return vignette;}\n"
"void main(){\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"float radius=0.68-(0.05*Intensity);\n"
"float softness=0.01+(0.3*Intensity);\n"
"float vignette=SquaredVignette(In.TextureCoord.xy,radius,softness);\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb *= vignette;\n"
"FragColor=dummy+vec4(color.rgb,1.0);}\n";
