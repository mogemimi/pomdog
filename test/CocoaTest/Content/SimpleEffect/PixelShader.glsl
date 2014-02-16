#version 330

in VertexData {
	vec2 TextureCoord;
} In;

uniform TestStructure {
	vec2 Rotation;
};

out vec4 FragColor;

void main()
{
	FragColor = vec4(In.TextureCoord.xy, Rotation.y, 1.0) * Rotation.x;
}
