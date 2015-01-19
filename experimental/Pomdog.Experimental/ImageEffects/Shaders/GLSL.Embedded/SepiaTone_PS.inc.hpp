//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

char const* Builtin_GLSL_SepiaTone_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;}In;\n"
"uniform Constants{\n"
"vec2 RenderTargetPixelSize;};\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"vec3 ComputeSepiaTone(vec3 color){\n"
"const vec3 r=vec3(0.393,0.769,0.189);\n"
"const vec3 g=vec3(0.349,0.686,0.168);\n"
"const vec3 b=vec3(0.272,0.534,0.131);\n"
"return clamp(vec3(dot(color,r),dot(color,g),dot(color,b)),0.0,1.0);}\n"
"void main(){\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"color.rgb=ComputeSepiaTone(color.rgb);\n"
"float dummy=(0.0*(RenderTargetPixelSize.x/RenderTargetPixelSize.y));\n"
"FragColor=dummy+color;}\n";
