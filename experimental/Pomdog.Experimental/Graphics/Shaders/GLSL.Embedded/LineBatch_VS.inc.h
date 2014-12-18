//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

char const* Builtin_GLSL_LineBatch_VS =
"#version 330\n"
"layout(location=0)in vec3 Position;\n"
"layout(location=1)in vec4 Color;\n"
"out VertexData{\n"
"vec4 DestinationColor;\n"
"}Out;\n"
"uniform TransformMatrix{\n"
"mat4 ViewProjection;\n"
"};\n"
"void main()\n"
"{\n"
"gl_Position=vec4(Position.xyz,1.0)*ViewProjection;\n"
"Out.DestinationColor=Color;\n"
"}\n";
