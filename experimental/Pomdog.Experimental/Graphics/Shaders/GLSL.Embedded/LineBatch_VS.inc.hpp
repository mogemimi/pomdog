// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

char const* Builtin_GLSL_LineBatch_VS =
"#version 330\n"
"layout(location=0)in vec3 Position;\n"
"layout(location=1)in vec4 Color;\n"
"out VertexData{\n"
"vec4 DestinationColor;}Out;\n"
"uniform TransformMatrix{\n"
"mat4 ViewProjection;};\n"
"void main(){\n"
"gl_Position=vec4(Position.xyz,1.0)*ViewProjection;\n"
"Out.DestinationColor=Color;}\n";
