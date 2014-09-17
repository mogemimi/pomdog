#version 330

in QuadVertexShaderOutput {
	vec2 TextureCoord;
} In;

uniform Constants {
	vec2 RenderTargetPixelSize;
};

//uniform VignetteProps {
//	float Intensity;
//};

uniform sampler2D Texture;

out vec4 FragColor;

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
float CircleVignette(vec2 textureCoord, float radius, float softness)
{
	// NOTE:
	// [0, 1] => [-0.5, +0.5]
	vec2 position = In.TextureCoord.xy - vec2(0.5);
	float len = length(position);
	float vignette = smoothstep(radius, radius - softness, len);
	return vignette;
}

///@param radius [0.0, 1.0], default: 0.5
///@param softness [0.0, 1.0], default: 0.05
///@param scale [0.0, 1.0] default: 1.0
float ScaledCircleVignette(vec2 textureCoord, float radius, float softness, vec2 scale)
{
	vec2 position = (In.TextureCoord.xy - vec2(0.5)) * scale;
	float len = length(position);
	float vignette = smoothstep(radius, radius - softness, len);
	return vignette;
}

float OldTVVignette(vec2 textureCoord)
{
	const float radius = 0.516;
	const float softness = 0.015;

	float vignette = ScaledCircleVignette(In.TextureCoord.xy, radius, softness, vec2(1.0, 0.27))
		* ScaledCircleVignette(In.TextureCoord.xy, radius, softness, vec2(0.5, 0.9));
	return vignette;
}

// Linear Burn blending
vec3 ApplyLinearBurn(vec3 base, float blend)
{
	return max(base + blend - 1.0, 0.0);
}

void main()
{
	float dummy = (0.0 * (RenderTargetPixelSize.x / RenderTargetPixelSize.y));
	
	const float radius = 0.9;
	const float softness = 0.45;
	
	float vignette = CircleVignette(In.TextureCoord.xy, radius, softness);
	float vignetteTV = OldTVVignette(In.TextureCoord.xy);
	
	vec4 color = texture(Texture, In.TextureCoord.xy);
	
	// Multiply blending
	color.rgb *= vignette;

	color.rgb = ApplyLinearBurn(color.rgb, vignetteTV);

	FragColor = dummy + vec4(color.rgb, 1.0);
}
