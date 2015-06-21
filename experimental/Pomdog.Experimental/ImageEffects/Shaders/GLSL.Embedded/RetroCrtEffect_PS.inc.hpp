// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

char const* Builtin_GLSL_RetroCrtEffect_PS =
"#version 330\n"
"in QuadVertexShaderOutput{\n"
"vec2 TextureCoord;}In;\n"
"uniform ImageEffectConstants{\n"
"vec2 RenderTargetPixelSize;\n"
"vec2 RcpFrame;};\n"
"uniform sampler2D AlbedoSampler;\n"
"out vec4 FragColor;\n"
"vec3 RetroCrtEffect(in vec2 texCoord,in vec2 resolution){\n"
"const float blendFactor=0.1; vec3 rgb=texture(AlbedoSampler,texCoord).rgb;\n"
"float artifact=cos(texCoord.y*resolution.y*2.0)*0.5+0.5;\n"
"return max(rgb-vec3(artifact*blendFactor),vec3(0.0));}\n"
"void main(){\n"
"vec3 color=RetroCrtEffect(In.TextureCoord.xy,RenderTargetPixelSize);\n"
"FragColor=vec4(color.xyz,1.0);}\n";
