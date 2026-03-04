#version 330

in QuadVertexShaderOutput {
    vec2 TextureCoord;
} In;

uniform ImageEffectConstants {
    vec2 RenderTargetPixelSize;
    vec2 RcpFrame;
};

uniform VignetteBlock {
    float Intensity;
};

uniform sampler2D Texture;

out vec4 FragColor;

/////@param radius [0.0, 1.0], default: 0.5
/////@param softness [0.0, 1.0], default: 0.05
//float CircleVignette(vec2 textureCoord, float radius, float softness)
//{
//    // NOTE: [0, 1] => [-0.5, +0.5]
//    vec2 position = textureCoord - vec2(0.5);
//    float len = length(position);
//    return smoothstep(radius, radius - softness, len);
//}

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
float SquaredVignette(vec2 textureCoord, float radius, float softness)
{
    // NOTE: [0, 1] => [-0.5, +0.5]
    vec2 position = textureCoord - vec2(0.5);
    vec2 squaredPos = position * position;
    float len = sqrt(length(squaredPos));
    return smoothstep(radius, radius - softness, len);
}

//// Linear Burn blending
//vec3 ApplyLinearBurn(vec3 base, float blend)
//{
//    return max(base + blend - 1.0, 0.0);
//}

void main()
{
    float dummy = (0.0 * (RenderTargetPixelSize.x / RenderTargetPixelSize.y));

    float radius = 0.68 - (0.05 * Intensity);
    float softness = 0.01 + (0.3 * Intensity);

    float vignette = SquaredVignette(In.TextureCoord.xy, radius, softness);

    vec4 color = texture(Texture, In.TextureCoord.xy);

    // Multiply blending
    color.rgb *= vignette;

    //color.rgb = ApplyLinearBurn(color.rgb, vignette);

    FragColor = dummy + vec4(color.rgb, 1.0);
}
