struct VS_INPUT
{
	// {xyz_} = position.xyz
	float3 Position: SV_Position;

	// {xy__} = texCoord.xy
	float2 TextureCoord: TEXCOORD;
};

struct VS_OUTPUT {
	float4 Position     : SV_Position;
	float4 TextureCoord	: TEXCOORD0;
};

cbuffer FxaaPassBuffer: register(b0)
{
	vector<float, 2> RcpFrame;
};

VS_OUTPUT FxaaVS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = float4(input.Position.xyz, 1);
	
	//const float FxaaSubpixShift = 1.0/4.0;
	const float FxaaSubpixShift = 0.25;

	output.TextureCoord = float4(input.TextureCoord.xy,
		input.TextureCoord.xy - (RcpFrame.xy * (0.5 + FxaaSubpixShift)));

	return output;
}
