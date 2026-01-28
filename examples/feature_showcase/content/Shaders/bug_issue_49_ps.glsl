#version 330

in VertexData {
    vec2 TextureCoord;
    vec4 Color;
} In;

uniform sampler2D DiffuseTexture; // NOTE: not [[texture(0)]] to reproduce the issue

out vec4 FragColor;

void main()
{
    vec4 color = texture(DiffuseTexture, In.TextureCoord.xy);
    FragColor = In.Color * color;
}
