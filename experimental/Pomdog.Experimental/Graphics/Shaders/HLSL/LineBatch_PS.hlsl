struct VS_OUTPUT {
    float4 Position         : SV_Position;
    float4 DestinationColor : COLOR0;
};

float4 LineBatchPS(VS_OUTPUT input) : SV_Target
{
    return input.DestinationColor;
}
