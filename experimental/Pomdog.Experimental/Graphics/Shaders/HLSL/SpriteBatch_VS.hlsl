struct VS_INPUT {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    float4 PositionTextureCoord : POSITION;

    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation          : SV_Position;
    // {xy__} = xy
    // {__zw} = {width, height}
    float4 SourceRect           : BINORMAL;
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = layerDepth
    float4 OriginRotationDepth  : NORMAL;
    // {xyzw} = color.rgba
    float4 Color                : COLOR;

    // per Instance
    uint InstanceID : SV_InstanceID;
};

struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float4 Color        : COLOR0;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer SpriteBatchConstants : register(b0) {
    matrix<float, 4, 4> ViewProjection;
    vector<float, 2>    InverseTextureSize;
};

VS_OUTPUT SpriteBatchVS(VS_INPUT input)
{
    float3x3 scaling = float3x3(
        float3(input.Translation.z * input.SourceRect.z, 0.0f, 0.0f),
        float3(0.0f, input.Translation.w * input.SourceRect.w, 0.0f),
        float3(0.0f, 0.0f, 1.0f));

    float cosRotation = cos(input.OriginRotationDepth.z);
    float sinRotation = sin(input.OriginRotationDepth.z);
    float3x3 rotate = float3x3(
        float3(cosRotation, sinRotation, 0.0f),
        float3(-sinRotation, cosRotation, 0.0f),
        float3(0.0f, 0.0f, 1.0f));

    float3x3 translate = float3x3(
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, 1.0f, 0.0f),
        float3(input.Translation.xy, 1.0f));

    float3x3 Transform = mul(mul(scaling, rotate), translate);

    float3 position = mul(float3(input.PositionTextureCoord.xy - input.OriginRotationDepth.xy, 1), Transform);

    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Position = float4(mul(float4(position.xy, 0, 1), ViewProjection).xy, input.OriginRotationDepth.w, 1);

    output.TextureCoord = (input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy) * InverseTextureSize.xy;

    output.Color = input.Color;

    return output;
}
