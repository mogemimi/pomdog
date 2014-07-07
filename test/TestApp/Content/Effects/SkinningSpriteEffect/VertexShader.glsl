#version 330

layout(location = 0) in vec4 PositionTextureCoord;
layout(location = 1) in vec4 Weights;
layout(location = 2) in ivec4 JointIndices;

out VertexData {
	vec2 TextureCoord;
	vec4 Color;
} Out;

uniform SkinningConstants {
	vec4 SkinMatrices1[64];
	vec4 SkinMatrices2[64];
};

uniform Constants {
	mat4x4 ModelViewProjection;
};

void main()
{
	mat3x2 skinning = mat3x2(0.0);
	for (int i = 0; i < 4; ++i)
	{
		int jointIndex = JointIndices[i];
		if (jointIndex < 0) {
			break;
		}

		mat3x2 boneMatrix = mat3x2(
			vec2(SkinMatrices1[jointIndex].xy),
			vec2(SkinMatrices1[jointIndex].zw),
			vec2(SkinMatrices2[jointIndex].xy));

		skinning += boneMatrix * Weights[i];
	}

	vec2 position = (skinning * vec3(PositionTextureCoord.xy, 1.0)).xy;

	gl_Position = vec4(position.xy, 0.0, 1.0) * ModelViewProjection;
	Out.TextureCoord = PositionTextureCoord.zw;
	Out.Color = vec4(1.0, 1.0, 1.0, 1.0);
}
