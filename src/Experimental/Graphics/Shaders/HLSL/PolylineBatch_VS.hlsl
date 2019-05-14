struct VS_INPUT {
    // {xyz_} = position.xyz
    // {___w} = unused
    float4 Position : SV_Position;

    // {xyz_} = nextPoint.xyz
    // {___w} = extrusion
    float4 NextPointExtrusion : NORMAL0;

    // {xyz_} = prevPoint.xyz
    // {___w} = lineThickness
    float4 PrevPointThickness : NORMAL1;

    // {xyzw} = color.rgba
    float4 Color : COLOR;
};

struct VS_OUTPUT {
    float4 Position : SV_Position;
    float4 DestinationColor : COLOR0;
};

cbuffer TransformMatrix: register(b0) {
    matrix<float, 4, 4> ViewProjection;
};

VS_OUTPUT PolylineBatchVS(VS_INPUT input)
{
    float lineWidth = input.PrevPointThickness.w;
    float extrusionDir = input.NextPointExtrusion.w;

    float4 currentProjected = mul(float4(input.Position.xyz, 1.0), ViewProjection);
    float4 nextProjected = mul(float4(input.NextPointExtrusion.xyz, 1.0), ViewProjection);
    float4 prevProjected = mul(float4(input.PrevPointThickness.xyz, 1.0), ViewProjection);

    float2 currentScreen = currentProjected.xy / currentProjected.w;
    float2 nextScreen = nextProjected.xy / nextProjected.w;
    float2 prevScreen = prevProjected.xy / prevProjected.w;

    float2 toNext = normalize(nextScreen.xy - currentScreen.xy);
    float2 normal = float2(-toNext.y, toNext.x);

    float2 fromPrev = normalize(currentScreen.xy - prevScreen.xy);
    float2 tangent = normalize(toNext + fromPrev);
    float2 miter = float2(-tangent.y, tangent.x);

    float thickness = lineWidth / dot(miter, normal);
    float4 offset = float4(miter.xy * extrusionDir * thickness, 0.0, 0.0);

    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = currentProjected + offset;
    output.DestinationColor = input.Color;
    return output;
}
