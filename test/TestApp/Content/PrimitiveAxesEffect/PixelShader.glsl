#version 330

in VertexData {
	vec3 DestinationColor;
} In;

out vec4 FragColor;

void main()
{
	FragColor = vec4(In.DestinationColor.xyz, 1.0);
}
