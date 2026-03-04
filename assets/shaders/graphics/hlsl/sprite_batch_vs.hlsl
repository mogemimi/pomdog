struct VS_INPUT {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    float4 PositionTextureCoord : POSITION;

    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy
    float4 Translation          : SV_Position;

    // per Instance
    // {xy__} = {rect.xy}
    // {__zw} = {rect.width, rect.height}
    float4 SourceRect           : BINORMAL;

    // per Instance
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = layerDepth
    float4 OriginRotationDepth  : NORMAL;

    // per Instance
    // {xyzw} = color.rgba
    float4 Color                : COLOR;

    // per Instance
    // {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
    // {__z_} = RGBA channel flags (8-bits)
    // {___w} = unused
    float4 InverseTextureSize   : TEXCOORD0;

    // per Instance
    uint InstanceID : SV_InstanceID;
};

struct VS_OUTPUT {
    float4 Position : SV_Position;
    float4 Color : COLOR0;

    // {x___} = RGB blend factor
    // {_y__} = Alpha blend factor
    // {__z_} = RGB compensation factor
    // {___w} = Alpha compensation factor
    float4 BlendFactor : COLOR1;

    float2 TextureCoord : TEXCOORD0;
};

cbuffer SpriteBatchConstants : register(b0) {
    matrix<float, 4, 4> ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    float4 DistanceFieldParameters;
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

    float3x3 transform = mul(mul(scaling, rotate), translate);

    float3 position = mul(float3(input.PositionTextureCoord.xy - input.OriginRotationDepth.xy, 1), transform);

    VS_OUTPUT output = (VS_OUTPUT)0;

    float4 finalPosition = mul(float4(position.xy, 0, 1), ViewProjection);

    float channelFlags = input.InverseTextureSize.z;
    bool sourceRGBEnabled = fmod(channelFlags, 2) == 1;
    bool sourceAlphaEnabled = fmod(channelFlags, 4) >= 2;
    bool compensationRGB = fmod(channelFlags, 8) >= 4;
    bool compensationAlpha = fmod(channelFlags, 16) >= 8;

    float4 blendFactor = float4(0.0, 0.0, 0.0, 0.0);
    if (sourceRGBEnabled) {
        blendFactor.x = 1.0;
    }
    if (sourceAlphaEnabled) {
        blendFactor.y = 1.0;
    }
    if (compensationRGB) {
        blendFactor.z = 1.0;
    }
    if (compensationAlpha) {
        blendFactor.w = 1.0;
    }

    output.Position = float4(finalPosition.xy, input.OriginRotationDepth.w, 1);
    output.TextureCoord = (input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy) * input.InverseTextureSize.xy;
    output.Color = input.Color;
    output.BlendFactor = blendFactor;

    return output;
}
