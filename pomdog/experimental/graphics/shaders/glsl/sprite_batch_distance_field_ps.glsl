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

uniform SpriteBatchConstants {
    mat4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    vec4 DistanceFieldParameters;
};

void main()
{
    float smoothing = DistanceFieldParameters.x;
    float weight = DistanceFieldParameters.y;

    float distance = texture(Texture, In.TextureCoord.xy).a;
    float alpha = smoothstep(weight - smoothing, weight + smoothing, distance);
    FragColor = vec4(In.Color.rgb, In.Color.a * alpha);
}
