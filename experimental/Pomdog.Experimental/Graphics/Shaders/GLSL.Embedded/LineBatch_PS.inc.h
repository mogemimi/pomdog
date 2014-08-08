//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

char const* Builtin_GLSL_LineBatch_PS =
"#version 330\n"
"in VertexData{\n"
"vec4 DestinationColor;\n"
"}In;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor=In.DestinationColor;\n"
"}\n";
