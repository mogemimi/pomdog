struct VS_INPUT {
    // {xyz_} = position.xyz
    float3 Position: SV_Position;

    // {xyzw} = color.rgba
    float4 Color: COLOR;
};

struct VS_OUTPUT {
    float4 Position         : SV_Position;
    float4 DestinationColor : COLOR0;
};

cbuffer TransformMatrix: register(b0) {
    matrix<float, 4, 4> ViewProjection;
};

VS_OUTPUT LineBatchVS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position.xyz, 1), ViewProjection);
    output.DestinationColor = input.Color;
    return output;
}
