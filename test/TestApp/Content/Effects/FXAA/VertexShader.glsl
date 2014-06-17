#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

uniform Constants {
	vec2 RenderTargetPixelSize;
};

out VertexData {
	vec4 TextureCoord;
} Out;

void main()
{
	gl_Position = vec4(Position.xyz, 1.0);
	vec2 rcpFrame = vec2(1.0, 1.0)/RenderTargetPixelSize.xy;
	
	const float FxaaSubpixShift = 1.0/4.0;
	Out.TextureCoord = vec4(TextureCoord.xy,
		TextureCoord.xy - (rcpFrame.xy * (0.5 + FxaaSubpixShift)));
}
