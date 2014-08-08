//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_FXAA_VS =
"#version 330\n"
"layout(location=0)in vec3 Position;\n"
"layout(location=1)in vec2 TextureCoord;\n"
"uniform Constants{\n"
"vec2 RenderTargetPixelSize;\n"
"};\n"
"out VertexData{\n"
"vec4 TextureCoord;\n"
"}Out;\n"
"void main()\n"
"{\n"
"gl_Position=vec4(Position.xyz,1.0);\n"
"vec2 rcpFrame=vec2(1.0,1.0)/RenderTargetPixelSize.xy;\n"
"const float FxaaSubpixShift=1.0/4.0;\n"
"Out.TextureCoord=vec4(TextureCoord.xy,\n"
"TextureCoord.xy-(rcpFrame.xy*(0.5+FxaaSubpixShift)));\n"
"}\n";
