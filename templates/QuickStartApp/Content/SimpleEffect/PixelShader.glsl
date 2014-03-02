#version 330

in VertexData {
	vec2 TextureCoord;
} In;

uniform TestStructure {
	vec2 Rotation;
};

uniform sampler2D DiffuseTexture;

out vec4 FragColor;

void main()
{
	FragColor = (vec4(In.TextureCoord.xy, Rotation.y, 1.0) * Rotation.x) * 0.5 +
		0.5 * texture(DiffuseTexture, In.TextureCoord.xy);
}
