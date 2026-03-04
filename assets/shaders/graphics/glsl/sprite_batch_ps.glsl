#version 330

in VertexData {
    vec4 Color;

    // {x___} = RGB blend factor
    // {_y__} = Alpha blend factor
    // {__z_} = RGB compensation factor
    // {___w} = Alpha compensation factor
    vec4 BlendFactor;

    vec2 TextureCoord;
} In;

uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(Texture, In.TextureCoord.xy);
    vec4 blendFactor = vec4(vec3(In.BlendFactor.x), In.BlendFactor.y);
    vec4 compensationFactor = vec4(vec3(In.BlendFactor.z), In.BlendFactor.w);
    color = min(color * blendFactor + compensationFactor, vec4(1.0));
    FragColor = color * In.Color.xyzw;
}
