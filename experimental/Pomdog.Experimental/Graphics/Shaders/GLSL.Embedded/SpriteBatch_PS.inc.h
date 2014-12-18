//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

char const* Builtin_GLSL_SpriteBatch_PS =
"#version 330\n"
"in VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;\n"
"}In;\n"
"uniform sampler2D DiffuseTexture;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor=texture(DiffuseTexture,In.TextureCoord.xy).xyzw*In.Color.xyzw;\n"
"}\n";
