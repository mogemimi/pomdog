// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

char const* Builtin_GLSL_SpriteBatchRenderer_VS =
"#version 330\n"
"layout(location=0)in vec4 PositionTextureCoord;\n"
"layout(location=1)in vec4 TransformMatrix1;\n"
"layout(location=2)in vec4 TransformMatrix2Origin;\n"
"layout(location=3)in vec4 SourceRect;\n"
"layout(location=4)in vec4 Color;\n"
"layout(location=5)in vec4 TextureSize;\n"
"out VertexData{\n"
"vec4 Color;\n"
"vec2 TextureCoord;}Out;\n"
"uniform SpriteBatchConstants{\n"
"mat4x4 ViewProjection;};\n"
"void main(){\n"
"mat2x2 scaling=mat2x2(\n"
"vec2(SourceRect.z,0.0),\n"
"vec2(0.0,SourceRect.w));\n"
"vec2 position=(PositionTextureCoord.xy-TransformMatrix2Origin.zw).xy*scaling;\n"
"mat3x2 worldMatrix=mat3x2(\n"
"vec2(TransformMatrix1.xy),\n"
"vec2(TransformMatrix1.zw),\n"
"vec2(TransformMatrix2Origin.xy));\n"
"position=(worldMatrix*vec3(position,1.0)).xy;\n"
"gl_Position=vec4(position.xy,0.0,1.0)*ViewProjection;\n"
"vec2 inverseTextureSize=TextureSize.xy;\n"
"Out.TextureCoord=(PositionTextureCoord.zw*SourceRect.zw+SourceRect.xy)*inverseTextureSize;\n"
"Out.Color=Color;}\n";
