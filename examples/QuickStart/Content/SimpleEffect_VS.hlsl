struct VS_INPUT
{
	// {xyz_} = position.xyz
	float3 Position: SV_Position;

	// {xy__} = texCoord.xy
	float2 TextureCoord: TEXCOORD;
};

struct VS_OUTPUT {
	float4 Position     : SV_Position;
	float2 TextureCoord	: TEXCOORD0;
};

VS_OUTPUT SimpleEffectVS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = float4(input.Position.xyz, 1);
	output.TextureCoord = input.TextureCoord.xy;
	return output;
}
