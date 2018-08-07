#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_INPUT {
    // {xyz_} = position.xyz
    float3 Position [[attribute(0)]];

    // {xy__} = texCoord.xy
    float2 TextureCoord [[attribute(1)]];
};

struct VS_OUTPUT {
    float4 Position [[position]];
    float4 TextureCoord;
};

struct __attribute__((__aligned__(256))) ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

vertex VS_OUTPUT FxaaVS(
    VS_INPUT input [[stage_in]],
    constant ImageEffectConstants& uniforms [[buffer(0)]])
{
    VS_OUTPUT output;

    output.Position = float4(input.Position.xyz, 1);

    //const float FxaaSubpixShift = 1.0/4.0;
    const float FxaaSubpixShift = 0.25;

    output.TextureCoord = float4(input.TextureCoord.xy,
        input.TextureCoord.xy - (uniforms.RcpFrame.xy * (0.5 + FxaaSubpixShift)));

    return output;
}

struct FxaaTex {
    texture2d<float> tex;
    sampler          smpl;
};

namespace {

float FxaaLuma(float3 rgb)
{
    const float3 luma = float3(0.299, 0.587, 0.114);
    return dot(rgb, luma);
}

float4 FxaaTexOff(FxaaTex tex, float2 pos, int2 offset)
{
    return tex.tex.sample(tex.smpl, pos.xy, level(0.0), offset.xy);
}

float4 FxaaTexLod(FxaaTex tex, float2 pos)
{
    return tex.tex.sample(tex.smpl, pos.xy, level(0.0));
}

float4 FxaaPixelShader(FxaaTex tex, float4 posPos, float2 rcpFrame)
{
    // NOTE:
    //     NW N NE
    //     W  M  E
    //     SW S SE

    float3 rgbNW = FxaaTexOff(tex, posPos.zw, int2(0, 0)).xyz;
    float3 rgbNE = FxaaTexOff(tex, posPos.zw, int2(1, 0)).xyz;
    float3 rgbSW = FxaaTexOff(tex, posPos.zw, int2(0, 1)).xyz;
    float3 rgbSE = FxaaTexOff(tex, posPos.zw, int2(1, 1)).xyz;
    float3 rgbM  = FxaaTexLod(tex, posPos.xy).xyz;

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

    const float FxaaReduceMin = 1.0/128.0;
    const float FxaaReduceMul = 1.0/8.0;
    const float FxaaSpanMax = 8.0;

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FxaaReduceMul),
        FxaaReduceMin);

    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, float2(-FxaaSpanMax, -FxaaSpanMax), float2(FxaaSpanMax, FxaaSpanMax)) * rcpFrame.xy;

    float3 rgbN1 = FxaaTexLod(tex, posPos.xy + dir * (1.0/3.0 - 0.5)).xyz;
    float3 rgbP1 = FxaaTexLod(tex, posPos.xy + dir * (2.0/3.0 - 0.5)).xyz;

    float3 rgbN2 = FxaaTexLod(tex, posPos.xy + dir * (0.0/3.0 - 0.5)).xyz;
    float3 rgbP2 = FxaaTexLod(tex, posPos.xy + dir * (3.0/3.0 - 0.5)).xyz;

    float3 rgbA = (rgbN1 + rgbP1);
    float3 rgbB = ((rgbN2 + rgbP2) * 0.25) + (rgbA * 0.25);

    float lumaB = FxaaLuma(rgbB);
    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
        rgbB.xyz = rgbA.xyz * 0.5;
    }

    return float4(rgbB, 1.0);
}

} // end of anonymous namespace

fragment float4 FxaaPS(
    VS_OUTPUT input [[stage_in]],
    constant ImageEffectConstants& uniforms [[buffer(0)]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]])
{
    FxaaTex fxaaTex = { diffuseTexture, textureSampler };
    return FxaaPixelShader(fxaaTex, input.TextureCoord, uniforms.RcpFrame);
}
