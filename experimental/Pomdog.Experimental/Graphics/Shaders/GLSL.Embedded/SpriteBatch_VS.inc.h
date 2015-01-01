//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

char const* Builtin_GLSL_SpriteBatch_VS =
"#version 330\n"
"layout(location=0)in vec4 PositionTextureCoord;\n"
"layout(location=1)in vec4 Translation;\n"
"layout(location=2)in vec4 SourceRect;\n"
"layout(location=3)in vec4 OriginRotationDepth;\n"
"layout(location=4)in vec4 Color;\n"
"out VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;\n"
"}Out;\n"
"uniform SpriteBatchConstants{\n"
"mat4x4 ViewProjection;\n"
"vec2 InverseTextureSize;\n"
"};\n"
"void main()\n"
"{\n"
"mat3x3 scaling=mat3x3(\n"
"vec3(Translation.z*SourceRect.z,0.0,0.0),\n"
"vec3(0.0,Translation.w*SourceRect.w,0.0),\n"
"vec3(0.0,0.0,1.0));\n"
"float cosRotation=cos(OriginRotationDepth.z);\n"
"float sinRotation=sin(OriginRotationDepth.z);\n"
"mat3x3 rotate=mat3x3(\n"
"vec3(cosRotation,-sinRotation,0.0),\n"
"vec3(sinRotation,cosRotation,0.0),\n"
"vec3(0.0f,0.0f,1.0));\n"
"mat3x3 translate=mat3x3(\n"
"vec3(1.0,0.0,Translation.x),\n"
"vec3(0.0,1.0,Translation.y),\n"
"vec3(0.0,0.0,1.0));\n"
"mat3x3 transform=(scaling*rotate)*translate;\n"
"vec3 position=(vec3(PositionTextureCoord.xy-OriginRotationDepth.xy,1.0)*transform);\n"
"gl_Position=vec4((vec4(position.xy,0.0,1.0)*ViewProjection).xy,OriginRotationDepth.w,1.0);\n"
"Out.TextureCoord=(PositionTextureCoord.zw*SourceRect.zw+SourceRect.xy)*InverseTextureSize.xy;\n"
"Out.Color=Color;\n"
"}\n";
