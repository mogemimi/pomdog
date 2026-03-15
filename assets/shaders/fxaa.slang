// Copyright mogemimi. Distributed under the MIT license.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> cb;

[[vk::binding(0, 1)]]
Texture2D<float4> DiffuseTexture;

[[vk::binding(1, 1)]]
SamplerState TextureSampler;

struct VSInput {
    [[vk::location(0)]] float3 Position;
    [[vk::location(1)]] float2 TextureCoord;
};

[shader("vertex")]
float4 fxaa_vs(
    VSInput input,
    [[vk::location(0)]] out float4 TextureCoord : TEXCOORD0
) : SV_Position
{
    //const float FxaaSubpixShift = 1.0/4.0;
    const float FxaaSubpixShift = 0.25;

    TextureCoord = float4(input.TextureCoord.xy,
        input.TextureCoord.xy - (cb.RcpFrame.xy * (0.5 + FxaaSubpixShift)));

    return float4(input.Position.xyz, 1.0);
}

float FxaaLuma(float3 rgb)
{
    const float3 luma = float3(0.299, 0.587, 0.114);
    return dot(rgb, luma);
}

float4 FxaaTexOff(Texture2D<float4> tex, SamplerState smpl, float2 pos, constexpr int2 offset)
{
    return tex.SampleLevel(smpl, pos.xy, 0.0, offset);
}

float4 FxaaTexLod(Texture2D<float4> tex, SamplerState smpl, float2 pos)
{
    return tex.SampleLevel(smpl, pos.xy, 0.0);
}

float4 FxaaPixelShader(Texture2D<float4> tex, SamplerState smpl, float4 posPos, float2 rcpFrame)
{
    // NOTE:
    //     NW N NE
    //     W  M  E
    //     SW S SE

    float3 rgbNW = FxaaTexOff(tex, smpl, posPos.zw, int2(0, 0)).xyz;
    float3 rgbNE = FxaaTexOff(tex, smpl, posPos.zw, int2(1, 0)).xyz;
    float3 rgbSW = FxaaTexOff(tex, smpl, posPos.zw, int2(0, 1)).xyz;
    float3 rgbSE = FxaaTexOff(tex, smpl, posPos.zw, int2(1, 1)).xyz;
    float3 rgbM  = FxaaTexLod(tex, smpl, posPos.xy).xyz;

    float lumaNW = FxaaLuma(rgbNW);
    float lumaNE = FxaaLuma(rgbNE);
    float lumaSW = FxaaLuma(rgbSW);
    float lumaSE = FxaaLuma(rgbSE);
    float lumaM  = FxaaLuma(rgbM);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    float dirSwMinusNe = lumaSW - lumaNE;
    float dirSeMinusNw = lumaSE - lumaNW;

    float2 dir = float2(dirSwMinusNe + dirSeMinusNw, dirSwMinusNe - dirSeMinusNw);

    const float FxaaReduceMin = 1.0 / 128.0;
    const float FxaaReduceMul = 1.0 / 8.0;
    const float FxaaSpanMax = 8.0;

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FxaaReduceMul),
        FxaaReduceMin);

    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, float2(-FxaaSpanMax, -FxaaSpanMax), float2(FxaaSpanMax, FxaaSpanMax)) * rcpFrame.xy;

    float3 rgbN1 = FxaaTexLod(tex, smpl, posPos.xy + dir * (1.0 / 3.0 - 0.5)).xyz;
    float3 rgbP1 = FxaaTexLod(tex, smpl, posPos.xy + dir * (2.0 / 3.0 - 0.5)).xyz;

    float3 rgbN2 = FxaaTexLod(tex, smpl, posPos.xy + dir * (0.0 / 3.0 - 0.5)).xyz;
    float3 rgbP2 = FxaaTexLod(tex, smpl, posPos.xy + dir * (3.0 / 3.0 - 0.5)).xyz;

    float3 rgbA = (rgbN1 + rgbP1);
    float3 rgbB = ((rgbN2 + rgbP2) * 0.25) + (rgbA * 0.25);

    float lumaB = FxaaLuma(rgbB);
    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        rgbB.xyz = rgbA.xyz * 0.5;
    }

    return float4(rgbB, 1.0);
}

[shader("pixel")]
float4 fxaa_ps(
    [[vk::location(0)]] float4 TextureCoord : TEXCOORD0
) : SV_Target0
{
    return FxaaPixelShader(DiffuseTexture, TextureSampler, TextureCoord, cb.RcpFrame);
}
