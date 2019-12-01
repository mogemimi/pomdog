#version 330

in VertexData {
    vec2 TextureCoord;
    vec4 Color;
} In;

uniform sampler2D DiffuseTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(DiffuseTexture, In.TextureCoord.xy).xyzw * In.Color.xyzw;

//    vec4 color = texture(DiffuseTexture, In.TextureCoord.xy).xyzw * In.Color.xyzw;
//    if (color.w < 0.7) {
//        color.w = 1.0;
//    }
//    FragColor = color;
}
