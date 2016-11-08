// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

char const* Builtin_GLSL_SpriteBatchRenderer_PS =
"#version 330\n"
"in VertexData{\n"
"vec4 Color;\n"
"vec2 TextureCoord;}In;\n"
"uniform sampler2D Texture;\n"
"out vec4 FragColor;\n"
"void main(){\n"
"vec4 color=texture(Texture,In.TextureCoord.xy);\n"
"FragColor=color*In.Color.xyzw;}\n";
