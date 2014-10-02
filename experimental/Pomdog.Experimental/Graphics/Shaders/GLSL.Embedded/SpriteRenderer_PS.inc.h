//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_SpriteRenderer_PS =
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
