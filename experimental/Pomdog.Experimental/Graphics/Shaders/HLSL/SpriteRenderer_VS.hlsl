struct VS_INPUT {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    float4 PositionTextureCoord: SV_Position;

    // per Instance
    // {x___} = worldMatrix.M00
    // {_y__} = worldMatrix.M01
    // {__z_} = worldMatrix.M10
    // {___w} = worldMatrix.M11
    float4 TransformMatrix1: POSITION0;

    // {x___} = worldMatrix.M20
    // {_y__} = worldMatrix.M21
    // {__z_} = layerDepth
    // {___w} = rotation
    float4 TransformMatrix2: POSITION1;

    // {xy__} = xy
    // {__zw} = {width, height}
    float4 SourceRect: BINORMAL;

    // {xy__} = originPivot.xy
    // {__zw} = scale.xy
    float4 OriginScale: NORMAL;

    // {xyzw} = color.rgba
    float4 Color: COLOR;

    // per Instance
    uint InstanceID: SV_InstanceID;
};

struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float4 Color        : COLOR0;
    float2 TextureCoord    : TEXCOORD0;
};

cbuffer Matrices: register(b0) {
    matrix<float, 4, 4> ViewProjection;
};

cbuffer TextureConstants: register(b1) {
    vector<float, 2> InverseTextureWidth;
};

VS_OUTPUT SpriteRendererVS(VS_INPUT input)
{
    float2x2 scaling = float2x2(
        float2(input.OriginScale.z * input.SourceRect.z, 0.0),
        float2(0.0, input.OriginScale.w * input.SourceRect.w));

    float cosRotation = cos(input.TransformMatrix2.w);
    float sinRotation = sin(input.TransformMatrix2.w);
    float2x2 rotate = float2x2(
        float2(cosRotation, sinRotation),
        float2(-sinRotation, cosRotation));

    float2x2 localTransform = mul(scaling, rotate);
    float2 position = mul((input.PositionTextureCoord.xy - input.OriginScale.xy).xy, localTransform);

    float2x2 worldMatrix = float2x2(
        float2(input.TransformMatrix1.xz),
        float2(input.TransformMatrix1.yw));
    position = mul(position, worldMatrix).xy + input.TransformMatrix2.xy;

    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = mul(float4(position.xy, 0.0, 1.0), ViewProjection);

    output.TextureCoord = (input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy) * InverseTextureWidth.xy;

    output.Color = input.Color;

    return output;
}
