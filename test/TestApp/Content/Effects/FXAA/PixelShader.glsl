#version 330

in VertexData {
	vec4 TextureCoord;
} In;

uniform Constants {
	vec2 RenderTargetPixelSize;
};

uniform sampler2D Texture;

out vec4 FragColor;

float FxaaLuma(vec3 rgb)
{
	const vec3 luma = vec3(0.299, 0.587, 0.114);
	return dot(rgb, luma);
}

vec4 FxaaPixelShader(sampler2D tex, vec4 posPos, vec2 rcpFrame)
{
	// NOTE:
	//     NW N NE
	//     W  M  E
	//     SW S SE

	vec3 rgbNW = textureLod(tex, posPos.zw, 0.0).xyz;
	vec3 rgbNE = textureLodOffset(tex, posPos.zw, 0.0, ivec2(1, 0)).xyz;
	vec3 rgbSW = textureLodOffset(tex, posPos.zw, 0.0, ivec2(0, 1)).xyz;
	vec3 rgbSE = textureLodOffset(tex, posPos.zw, 0.0, ivec2(1, 1)).xyz;
	vec3 rgbM  = textureLod(tex, posPos.xy, 0.0).xyz;

	float lumaNW = FxaaLuma(rgbNW);
	float lumaNE = FxaaLuma(rgbNE);
	float lumaSW = FxaaLuma(rgbSW);
	float lumaSE = FxaaLuma(rgbSE);
	float lumaM  = FxaaLuma(rgbM);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir = vec2(
		-((lumaNW + lumaNE) - (lumaSW + lumaSE)),
		 ((lumaNW + lumaSW) - (lumaNE + lumaSE)));

	const float FxaaReduceMin = 1.0/128.0;
	const float FxaaReduceMul = 1.0/8.0;
	const float FxaaSpanMax = 8.0;

	float dirReduce = max(
		(lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FxaaReduceMul),
		FxaaReduceMin);

	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FxaaSpanMax, FxaaSpanMax),
		max(vec2(-FxaaSpanMax, -FxaaSpanMax), dir * rcpDirMin)) * rcpFrame.xy;

	vec3 rgbA = (1.0/2.0) * (
		textureLod(tex, posPos.xy + dir * (1.0/3.0 - 0.5), 0.0).xyz +
		textureLod(tex, posPos.xy + dir * (2.0/3.0 - 0.5), 0.0).xyz);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		textureLod(tex, posPos.xy + dir * (0.0/3.0 - 0.5), 0.0).xyz +
		textureLod(tex, posPos.xy + dir * (3.0/3.0 - 0.5), 0.0).xyz);
	
	float lumaB = FxaaLuma(rgbB);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
		return vec4(rgbA, 1.0);
	}
	
	return vec4(rgbB, 1.0);
}

void main()
{
	//vec2 rcpFrame = 1.0/textureSize(Texture,0);
	vec2 rcpFrame = vec2(1.0, 1.0)/RenderTargetPixelSize.xy;
	FragColor = FxaaPixelShader(Texture, In.TextureCoord, rcpFrame);
	//FragColor = texture(Texture, position.xy).xyz;
}
