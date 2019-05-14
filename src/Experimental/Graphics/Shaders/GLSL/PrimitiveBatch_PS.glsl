#version 330

in VertexData {
    vec4 DestinationColor;
} In;

out vec4 FragColor;

void main()
{
    FragColor = In.DestinationColor;
}
