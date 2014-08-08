//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_SkinnedEffect_VS =
"#version 330\n"
"layout(location=0)in vec4 PositionTextureCoord;\n"
"layout(location=1)in vec4 Weights;\n"
"layout(location=2)in ivec4 JointIndices;\n"
"out VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;\n"
"}Out;\n"
"uniform SkinningConstants{\n"
"vec4 Bones[192];\n"
"};\n"
"uniform Constants{\n"
"mat4x4 ModelViewProjection;\n"
"};\n"
"void main()\n"
"{\n"
"mat3x2 skinning=mat3x2(0.0);\n"
"for(int i=0; i<4; ++i)\n"
"{\n"
"int jointIndex=JointIndices[i];\n"
"if(jointIndex<0){\n"
"break;\n"
"}\n"
"mat3x2 boneMatrix=mat3x2(\n"
"vec2(Bones[jointIndex*2].xy),\n"
"vec2(Bones[jointIndex*2].zw),\n"
"vec2(Bones[jointIndex*2+1].xy));\n"
"skinning += boneMatrix*Weights[i];\n"
"}\n"
"vec2 position=(skinning*vec3(PositionTextureCoord.xy,1.0)).xy;\n"
"gl_Position=vec4(position.xy,0.0,1.0)*ModelViewProjection;\n"
"Out.TextureCoord=PositionTextureCoord.zw;\n"
"Out.Color=vec4(1.0,1.0,1.0,1.0);\n"
"}\n";
