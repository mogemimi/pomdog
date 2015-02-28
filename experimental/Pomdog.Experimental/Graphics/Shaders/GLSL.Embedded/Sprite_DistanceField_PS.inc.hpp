//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

char const* Builtin_GLSL_Sprite_DistanceField_PS =
"#version 330\n"
"in VertexData{\n"
"vec2 TextureCoord;\n"
"vec4 Color;}In;\n"
"uniform sampler2D AlbedoTexture;\n"
"out vec4 FragColor;\n"
"uniform DistanceFieldConstants{\n"
"float Smoothing;\n"
"float Weight;};\n"
"void main(){\n"
"float distance=texture(AlbedoTexture,In.TextureCoord.xy).a;\n"
"float alpha=smoothstep(Weight-Smoothing,Weight+Smoothing,distance);\n"
"FragColor=vec4(In.Color.xyz,In.Color.w*alpha);}\n";
