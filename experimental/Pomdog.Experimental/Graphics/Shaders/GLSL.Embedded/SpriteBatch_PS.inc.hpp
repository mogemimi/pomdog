// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

char const* Builtin_GLSL_SpriteBatch_PS =
"#version 330\n"
"in VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;}In;\n"
"uniform sampler2D DiffuseTexture;\n"
"out vec4 FragColor;\n"
"void main(){\n"
"FragColor=texture(DiffuseTexture,In.TextureCoord.xy).xyzw*In.Color.xyzw;}\n";
