// Copyright mogemimi. Distributed under the MIT license.

struct SpriteBatchConstants {
    float4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    float4 DistanceFieldParameters;
};

[[vk::binding(0, 0)]]
ConstantBuffer<SpriteBatchConstants> cb;

[[vk::binding(0, 1)]]
Sampler2D in_Texture;

struct VSInput {
    // {xy__} = position.xy
    // {__zw} = texCoord.xy
    [[vk::location(0)]] float4 PositionTextureCoord;

    // per Instance
    // {xy__} = position.xy
    // {__zw} = scale.xy
    [[vk::location(1)]] float4 Translation;

    // per Instance
    // {xy__} = {rect.xy}
    // {__zw} = {rect.width, rect.height}
    [[vk::location(2)]] float4 SourceRect;

    // per Instance
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = layerDepth
    [[vk::location(3)]] float4 OriginRotationDepth;

    // per Instance
    // {xyzw} = color.rgba
    [[vk::location(4)]] float4 Color;

    // per Instance
    // {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
    // {__z_} = RGBA channel flags (8-bits)
    // {___w} = unused
    [[vk::location(5)]] float4 InverseTextureSize;
};

[shader("vertex")]
float4 sprite_batch_vs(
    VSInput input,
    [[vk::location(0)]] out float4 Color : COLOR0,
    [[vk::location(1)]] out float4 BlendFactor : COLOR1,
    [[vk::location(2)]] out float2 TextureCoord : TEXCOORD0
) : SV_Position
{
    float3x3 scaling = float3x3(
        float3(input.Translation.z * input.SourceRect.z, 0.0, 0.0),
        float3(0.0, input.Translation.w * input.SourceRect.w, 0.0),
        float3(0.0, 0.0, 1.0));

    float cosRotation = cos(input.OriginRotationDepth.z);
    float sinRotation = sin(input.OriginRotationDepth.z);
    float3x3 rotate = float3x3(
        float3(cosRotation, sinRotation, 0.0),
        float3(-sinRotation, cosRotation, 0.0),
        float3(0.0, 0.0, 1.0));

    float3x3 translate = float3x3(
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(input.Translation.xy, 1.0));

    float3x3 transform = mul(mul(scaling, rotate), translate);
    float3 position = mul(float3(input.PositionTextureCoord.xy - input.OriginRotationDepth.xy, 1.0), transform);

    // NOTE: 'ViewProjection' has already been transposed.
    float4 finalPosition = mul(float4(position.xy, 0.0, 1.0), cb.ViewProjection);

    float channelFlags = input.InverseTextureSize.z;
    bool sourceRGBEnabled = fmod(channelFlags, 2.0) == 1.0;
    bool sourceAlphaEnabled = fmod(channelFlags, 4.0) >= 2.0;
    bool compensationRGB = fmod(channelFlags, 8.0) >= 4.0;
    bool compensationAlpha = fmod(channelFlags, 16.0) >= 8.0;

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

    TextureCoord = (input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy) * input.InverseTextureSize.xy;
    Color = input.Color;
    BlendFactor = blendFactor;
    return float4(finalPosition.xy, input.OriginRotationDepth.w, 1.0);
}

[shader("pixel")]
float4 sprite_batch_ps(
    [[vk::location(0)]] float4 Color : COLOR0,
    [[vk::location(1)]] float4 BlendFactor : COLOR1,
    [[vk::location(2)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float4 color = in_Texture.Sample(TextureCoord.xy);
    float4 blendFactor = float4(float3(BlendFactor.x, BlendFactor.x, BlendFactor.x), BlendFactor.y);
    float4 compensationFactor = float4(float3(BlendFactor.z, BlendFactor.z, BlendFactor.z), BlendFactor.w);
    color = min(color * blendFactor + compensationFactor, float4(1.0, 1.0, 1.0, 1.0));
    return color * Color;
}
