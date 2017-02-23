// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

char const* Builtin_GLSL_Grayscale_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;}In;\n"
"uniform ImageEffectConstants{\n"
"vec2 RenderTargetPixelSize;\n"
"vec2 RcpFrame;};\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"float ComputeLuma(vec3 color){\n"
"const vec3 luma=vec3(0.299,0.587,0.114);\n"
"return dot(color,luma);}\n"
"vec3 ComputeGrayscale(vec3 color){\n"
"return vec3(ComputeLuma(color));}\n"
"void main(){\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb=ComputeGrayscale(color.rgb);\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"FragColor=dummy+color;}\n";
