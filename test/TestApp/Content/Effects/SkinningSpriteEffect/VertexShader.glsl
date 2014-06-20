#version 330

layout(location = 0) in vec4 PositionTextureCoord;
layout(location = 1) in vec4 Weights;
layout(location = 2) in ivec4 JointIndices;

out VertexData {
	vec2 TextureCoord;
	vec4 Color;
} Out;

uniform SkinningConstants {
	vec4 SkinMatrices1[32];
	vec4 SkinMatrices2[32];
};

uniform Constants {
	mat4x4 ViewProjection;
};

void main()
{
	vec2 position = vec2(0.0, 0.0);
	
	for (int i = 0; i < 4; ++i)
	{
		int jointIndex = JointIndices[i];
		if (jointIndex < 0) {
			break;
		}

		mat3x3 skinningMatrix = transpose(mat3x3(
			vec3(SkinMatrices1[jointIndex].xy, 0),
			vec3(SkinMatrices1[jointIndex].zw, 0),
			vec3(SkinMatrices2[jointIndex].xy, 1)));

		position = position + (vec3(PositionTextureCoord.xy, 1.0) * skinningMatrix).xy * Weights[i];
	}

	//position = PositionTextureCoord.xy;
	
	gl_Position = vec4(position.xy, 0.0, 1.0) * ViewProjection;
	Out.TextureCoord = PositionTextureCoord.zw;
	Out.Color = vec4(1.0, 1.0, 1.0, 1.0);
}
