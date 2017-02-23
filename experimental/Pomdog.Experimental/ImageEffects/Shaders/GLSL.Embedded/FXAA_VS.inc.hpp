// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

char const* Builtin_GLSL_FXAA_VS =
"#version 330\n"
"layout(location=0)in vec3 Position;\n"
"layout(location=1)in vec2 TextureCoord;\n"
"uniform ImageEffectConstants{\n"
"vec2 RenderTargetPixelSize;\n"
"vec2 RcpFrame;};\n"
"out VertexData{\n"
"vec4 TextureCoord;}Out;\n"
"void main(){\n"
"gl_Position=vec4(Position.xyz,1.0);\n"
"const float FxaaSubpixShift=0.25;\n"
"Out.TextureCoord=vec4(TextureCoord.xy,\n"
"TextureCoord.xy-(RcpFrame.xy*(0.5+FxaaSubpixShift)));}\n";
