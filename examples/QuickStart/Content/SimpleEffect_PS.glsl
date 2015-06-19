#version 330

in VertexData {
    vec2 TextureCoord;
} In;

uniform MyConstants {
    vec2 Rotation;
};

uniform sampler2D DiffuseTexture;

out vec4 FragColor;

void main()
{
    vec4 rotationColor = vec4(In.TextureCoord.xy, Rotation.y, 1.0) * Rotation.x;
    vec4 textureColor = texture(DiffuseTexture, In.TextureCoord.xy);
    FragColor = mix(rotationColor, textureColor, 0.5);
}
