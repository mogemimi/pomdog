#version 330

in QuadVertexShaderOutput {
    vec2 TextureCoord;
} In;

uniform ImageEffectConstants {
    vec2 RenderTargetPixelSize;
    vec2 RcpFrame;
};

//uniform GrayscaleProps {
//    float Density;
//};

uniform sampler2D Texture;

out vec4 FragColor;

float ComputeLuma(vec3 color)
{
    const vec3 luma = vec3(0.299, 0.587, 0.114);
    return dot(color, luma);
}

vec3 ComputeGrayscale(vec3 color)
{
    return vec3(ComputeLuma(color));
}

void main()
{
    vec4 color = texture(Texture, In.TextureCoord.xy);
    color.rgb = ComputeGrayscale(color.rgb);

    float dummy = (0.0 * (RenderTargetPixelSize.x / RenderTargetPixelSize.y));
    FragColor = dummy + color;
}
