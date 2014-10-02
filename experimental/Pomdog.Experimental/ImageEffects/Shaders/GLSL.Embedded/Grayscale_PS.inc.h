//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_Grayscale_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;\n"
"}In;\n"
"uniform Constants{\n"
"vec2 RenderTargetPixelSize;\n"
"};\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"float ComputeLuma(vec3 color)\n"
"{\n"
"const vec3 luma=vec3(0.299,0.587,0.114);\n"
"return dot(color,luma);\n"
"}\n"
"vec3 ComputeGrayscale(vec3 color)\n"
"{\n"
"return vec3(ComputeLuma(color));\n"
"}\n"
"void main(){\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb=ComputeGrayscale(color.rgb);\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"FragColor=dummy+color;\n"
"}\n";
