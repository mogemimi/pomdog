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
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"float CircleVignette(vec2 textureCoord,float radius,float softness)\n"
"{\n"
"vec2 position=In.TextureCoord.xy-vec2(0.5);\n"
"float len=length(position);\n"
"float vignette=smoothstep(radius,radius-softness,len);\n"
"return vignette;\n"
"}\n"
"float ScaledCircleVignette(vec2 textureCoord,float radius,float softness,vec2 scale)\n"
"{\n"
"vec2 position=(In.TextureCoord.xy-vec2(0.5))*scale;\n"
"float len=length(position);\n"
"float vignette=smoothstep(radius,radius-softness,len);\n"
"return vignette;\n"
"}\n"
"void main(){\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"const float radius=0.9;\n"
"const float softness=0.45;\n"
"float vignette=CircleVignette(In.TextureCoord.xy,radius,softness);\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb *= vignette;\n"
"FragColor=dummy+vec4(color.rgb,1.0);\n"
"}\n";
