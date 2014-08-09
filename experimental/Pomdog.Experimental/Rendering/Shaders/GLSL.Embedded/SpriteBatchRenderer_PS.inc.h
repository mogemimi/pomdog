//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_SpriteBatchRenderer_PS =
"#version 330\n"
"in VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;\n"
"float TextureIndex;\n"
"}In;\n"
"#if __VERSION__<420\n"
"uniform sampler2D Texture0;\n"
"uniform sampler2D Texture1;\n"
"uniform sampler2D Texture2;\n"
"uniform sampler2D Texture3;\n"
"#else\n"
"layout(binding=0)uniform sampler2D Texture0;\n"
"layout(binding=1)uniform sampler2D Texture1;\n"
"layout(binding=2)uniform sampler2D Texture2;\n"
"layout(binding=3)uniform sampler2D Texture3;\n"
"#endif\n"
"out vec4 FragColor;\n"
"vec4 FindTextureColor(int textureIndex,vec2 uv)\n"
"{\n"
"if(textureIndex==0){\n"
"return texture(Texture0,uv);\n"
"}\n"
"else if(textureIndex==1){\n"
"return texture(Texture1,uv);\n"
"}\n"
"else if(textureIndex==2){\n"
"return texture(Texture2,uv);\n"
"}\n"
"return texture(Texture3,uv);\n"
"}\n"
"void main()\n"
"{\n"
"int textureIndex=clamp(int(In.TextureIndex),0,3);\n"
"FragColor=FindTextureColor(textureIndex,In.TextureCoord.xy).xyzw*In.Color.xyzw;\n"
"}\n";
