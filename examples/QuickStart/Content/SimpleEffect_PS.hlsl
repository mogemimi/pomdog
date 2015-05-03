struct VS_OUTPUT {
	float4 Position     : SV_Position;
	float2 TextureCoord	: TEXCOORD0;
};

cbuffer MyConstants: register(b0)
{
	vector<float, 2> Rotation;
};

Texture2D<float4> DiffuseTexture: register(t0);
SamplerState      TextureSampler: register(s0);

float4 SimpleEffectPS(VS_OUTPUT input) : SV_Target
{
    float4 rotationColor = float4(input.TextureCoord.xy, Rotation.y, 1.0) * Rotation.x;
    float4 textureColor = DiffuseTexture.Sample(TextureSampler, input.TextureCoord.xy);
	return lerp(rotationColor, textureColor, 0.5);
}
