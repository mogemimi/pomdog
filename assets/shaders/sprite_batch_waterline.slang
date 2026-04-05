// Copyright mogemimi. Distributed under the MIT license.

struct SpriteBatchConstants {
    float4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__z_} = OutlineWeight
    // {___w} = unused
    float4 DistanceFieldParameters;

    // {rgba} = OutlineColor
    float4 OutlineColor;
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
    // {__zw} = scale.xy * sourceRect.{width, height}
    [[vk::location(1)]] float4 Translation;

    // per Instance
    // {xy__} = {rect.xy * inverseTextureSize}
    // {__zw} = {rect.width * inverseTextureSize, rect.height * inverseTextureSize}
    [[vk::location(2)]] float4 SourceRect;

    // per Instance
    // {xy__} = originPivot.xy
    // {__z_} = rotation
    // {___w} = waterLineYPosition
    [[vk::location(3)]] float4 OriginRotationWaterLine;

    // per Instance
    // {xyzw} = color0.rgba (Unorm8x4)
    [[vk::location(4)]] float4 Color0;

    // per Instance
    // {xyzw} = color1.rgba (Unorm8x4)
    [[vk::location(5)]] float4 Color1;

    // per Instance
    // {xyzw} = color2.rgba (Unorm8x4)
    [[vk::location(6)]] float4 Color2;

    // per Instance
    // {xy__} = reserved (Unorm8x2)
    // {__z_} = blendFactor0 (Unorm8x1)
    // {___w} = blendFactor1 (Unorm8x1)
    [[vk::location(7)]] float4 ReservedBlendFactor;
};

[shader("vertex")]
float4 sprite_batch_waterline_vs(
    VSInput input,
    [[vk::location(0)]] out float4 Color0 : COLOR0,
    [[vk::location(1)]] out float4 Color1 : COLOR1,
    [[vk::location(2)]] out float2 TextureCoord : TEXCOORD0,
    [[vk::location(3)]] out float BlendFactor0 : TEXCOORD1,
    [[vk::location(4)]] out float BlendFactor1 : TEXCOORD2,
    [[vk::location(5)]] out float WorldY : TEXCOORD3,
    [[vk::location(6)]] out float WaterLine : TEXCOORD4
) : SV_Position
{
    float3x3 scaling = float3x3(
        float3(input.Translation.z, 0.0, 0.0),
        float3(0.0, input.Translation.w, 0.0),
        float3(0.0, 0.0, 1.0));

    float cosRotation = cos(input.OriginRotationWaterLine.z);
    float sinRotation = sin(input.OriginRotationWaterLine.z);
    float3x3 rotate = float3x3(
        float3(cosRotation, sinRotation, 0.0),
        float3(-sinRotation, cosRotation, 0.0),
        float3(0.0, 0.0, 1.0));

    float3x3 translate = float3x3(
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(input.Translation.xy, 1.0));

    float3x3 transform = mul(mul(scaling, rotate), translate);
    float3 position = mul(float3(input.PositionTextureCoord.xy - input.OriginRotationWaterLine.xy, 1.0), transform);

    // NOTE: 'ViewProjection' has already been transposed.
    float4 finalPosition = mul(float4(position.xy, 0.0, 1.0), cb.ViewProjection);

    Color0 = input.Color0;
    Color1 = input.Color1;
    TextureCoord = input.PositionTextureCoord.zw * input.SourceRect.zw + input.SourceRect.xy;
    BlendFactor0 = input.ReservedBlendFactor.z;
    BlendFactor1 = input.ReservedBlendFactor.w;
    WorldY = position.y;
    WaterLine = input.OriginRotationWaterLine.w;
    return float4(finalPosition.xy, 0.0, 1.0);
}

[shader("pixel")]
float4 sprite_batch_waterline_ps(
    [[vk::location(0)]] float4 Color0 : COLOR0,
    [[vk::location(1)]] float4 Color1 : COLOR1,
    [[vk::location(2)]] float2 TextureCoord : TEXCOORD0,
    [[vk::location(3)]] float BlendFactor0 : TEXCOORD1,
    [[vk::location(4)]] float BlendFactor1 : TEXCOORD2,
    [[vk::location(5)]] float WorldY : TEXCOORD3,
    [[vk::location(6)]] float WaterLine : TEXCOORD4
) : SV_Target0
{
    float4 texColor = in_Texture.Sample(TextureCoord.xy);

    // NOTE: 5px is hard-coded value for smoothing
    float blend = smoothstep(WaterLine - 5.0, WaterLine, WorldY);
    float4 vfxColor = lerp(Color1, Color0, blend);
    float solidFillBlendFactor = lerp(BlendFactor1, BlendFactor0, blend);

    float4 mixed = lerp(
        texColor * vfxColor,
        float4(vfxColor.rgb * texColor.a, texColor.a),
        solidFillBlendFactor);

    return mixed;
}
