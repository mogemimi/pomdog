#version 330

in VertexData {
	vec2 TextureCoord;
	vec4 Color;
} In;

uniform sampler2D DiffuseTexture;

out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(DiffuseTexture, In.TextureCoord.xy).xyz * In.Color.xyz, In.Color.w);
}
