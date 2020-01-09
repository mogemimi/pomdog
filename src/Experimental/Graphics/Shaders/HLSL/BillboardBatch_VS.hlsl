struct VertexInput {
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    float4 PositionTextureCoord : POSITION;

    // {xyz_} = position.xyz
    // {___w} = rotation.z
    float4 Translation : SV_Position;

    // {xy__} = textureCoord.xy
    // {__zw} = textureSize.xy
    float4 TextureCoordRect : TEXCOORD0;

    // {xy__} = originPivot.xy
    // {__zw} = scale.xy
    float4 OriginPivotScale : NORMAL;

    // {xyz_} = color.rgb
    // {___w} = color.a
    float4 Color : COLOR;
};

struct VertexOutput {
    float4 Position : SV_Position;
    float4 Color : COLOR0;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer WorldConstants: register(b0) {
    matrix<float, 4, 4> ViewProjection;
    matrix<float, 4, 4> View;
    matrix<float, 4, 4> Projection;
    matrix<float, 4, 4> InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    float4 LightDirection;
};

VertexOutput BillboardBatchVS(VertexInput input)
{
    float2 scale = input.OriginPivotScale.zw;
    float4x4 scaling = float4x4(
        float4(scale.x, 0.0, 0.0, 0.0),
        float4(0.0, scale.y, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float rotationZ = input.Translation.w;
    float cosRotation = cos(rotationZ);
    float sinRotation = sin(rotationZ);
    float4x4 rotate = float4x4(
        float4(cosRotation, sinRotation, 0.0, 0.0),
        float4(-sinRotation, cosRotation, 0.0, 0.0),
        float4(0.0, 0.0, 1.0, 0.0),
        float4(0.0, 0.0, 0.0, 1.0));

    float4x4 transform = mul(rotate, scaling);
    float4 worldSpacePosition = mul(transform, float4(input.PositionTextureCoord.xy - input.OriginPivotScale.xy, 0.0, 1.0));
    float4 viewSpaceOffset = mul(View, float4(input.Translation.xyz, 1.0));

    VertexOutput output = (VertexOutput)0;
    output.Position = mul(Projection, float4(worldSpacePosition.xyz + viewSpaceOffset.xyz, 1.0));
    output.TextureCoord = input.PositionTextureCoord.zw * input.TextureCoordRect.zw + input.TextureCoordRect.xy;
    output.Color = input.Color;
    return output;
}
