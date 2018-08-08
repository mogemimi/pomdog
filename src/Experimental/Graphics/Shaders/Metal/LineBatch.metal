#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct __attribute__((__aligned__(256))) TransformMatrix {
    matrix_float4x4 ViewProjection;
};

struct VS_INPUT {
    // {xyz_} = position.xyz
    float3 Position [[attribute(0)]];

    // {xyzw} = color.rgba
    float4 Color [[attribute(1)]];
};

struct VS_OUTPUT {
    float4 Position [[position]];
    float4 DestinationColor;
};

vertex VS_OUTPUT LineBatchVS(
    VS_INPUT input [[stage_in]],
    constant TransformMatrix& uniforms [[buffer(0)]])
{
    VS_OUTPUT out;
    out.Position = float4(input.Position, 1.0) * uniforms.ViewProjection;
    out.DestinationColor = input.Color;
    return out;
}

fragment float4 LineBatchPS(VS_OUTPUT input [[stage_in]])
{
    return input.DestinationColor;
}
