#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_OUTPUT {
    float4 Position [[position]];
    float2 TextureCoord;
};

struct __attribute__((__aligned__(256))) ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

struct __attribute__((__aligned__(256))) FishEyeBlock {
    float Strength;
};

namespace {

constexpr constant float PI = 3.1415926535;

#if 0
float2 BasicFishEye(float2 texCoord)
{
    const float aperture = 178.0;
    float apertureHalf = 0.5 * aperture * (PI/ 180.0);
    float maxFactor = sin(apertureHalf);

    float2 xy = 2.0 * texCoord - 1.0;
    float d = length(xy);

    float2 uv = texCoord;

    if (d < (2.0 - maxFactor)) {
        d = length(xy * maxFactor);
        float z = sqrt(1.0 - d * d);
        float r = atan2(d, z) / PI;
        float phi = atan2(xy.y, xy.x);

        uv.x = r * cos(phi) + 0.5;
        uv.y = r * sin(phi) + 0.5;
    }

    return uv;
}
#endif

///@param strength range: (-0.5, 0.5)
float2 FishEye(float2 texCoord, float2 resolution, float strength)
{
    float2 p = float2(texCoord.x,  texCoord.y * resolution.y/resolution.x);

    float prop = resolution.x / resolution.y;
    float2 m = float2(0.5, 0.5 / prop);

    float2 d = p - m;

    float r = sqrt(dot(d, d));

    float power = (PI / sqrt(dot(m, m))) * strength;

    float bind = m.y;
    if (power > 0.0) {
        bind = sqrt(dot(m, m));
    }
    else {
        if (prop < 1.0) {
            bind = m.x;
        } else {
            bind = m.y;
        }
    }

    float2 uv;
    if (power > 0.0) {
        // fisheye
        uv = m + normalize(d) * tan(r * power) * bind / tan(bind * power);
    } else if (power < 0.0) {
        // anti-fisheye
        uv = m + normalize(d) * atan(r * -power * 10.0) * bind / atan(-power * bind * 10.0);
    } else {
        // no effect
        uv = p;
    }

    uv.y = uv.y * prop;
    return uv;
}

} // end of anonymous namespace

fragment float4 FishEyePS(
    VS_OUTPUT input [[stage_in]],
    constant ImageEffectConstants& uniforms [[buffer(0)]],
    constant FishEyeBlock& fishEyeUniforms [[buffer(1)]],
    texture2d<float> diffuseTexture [[texture(0)]],
    sampler textureSampler [[sampler(0)]])
{
    float2 uv = FishEye(input.TextureCoord.xy, uniforms.RenderTargetPixelSize.xy, fishEyeUniforms.Strength);
    float4 color = diffuseTexture.sample(textureSampler, uv);
    return color;
}
