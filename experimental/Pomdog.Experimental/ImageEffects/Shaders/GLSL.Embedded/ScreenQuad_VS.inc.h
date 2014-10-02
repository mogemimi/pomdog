//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_ScreenQuad_VS =
"#version 330\n"
"layout(location=0)in vec3 Position;\n"
"layout(location=1)in vec2 TextureCoord;\n"
"out QuadVertexShaderOutput{\n"
"vec2 TextureCoord;\n"
"}Out;\n"
"void main(){\n"
"gl_Position=vec4(Position.xyz,1.0);\n"
"Out.TextureCoord=TextureCoord.xy;\n"
"}\n";
