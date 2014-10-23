//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_Vignette_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;\n"
"}In;\n"
"uniform Constants{\n"
"vec2 RenderTargetPixelSize;\n"
"};\n"
"uniform VignetteBlock{\n"
"float Intensity;\n"
"};\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"float SquaredVignette(vec2 textureCoord,float radius,float softness)\n"
"{\n"
"vec2 position=textureCoord-vec2(0.5);\n"
"vec2 squaredPos=position*position;\n"
"float len=sqrt(length(squaredPos));\n"
"float vignette=smoothstep(radius,radius-softness,len);\n"
"return vignette;\n"
"}\n"
"void main(){\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"float radius=0.68-(0.05*Intensity);\n"
"float softness=0.01+(0.3*Intensity);\n"
"float vignette=SquaredVignette(In.TextureCoord.xy,radius,softness);\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb *= vignette;\n"
"FragColor=dummy+vec4(color.rgb,1.0);\n"
"}\n";
