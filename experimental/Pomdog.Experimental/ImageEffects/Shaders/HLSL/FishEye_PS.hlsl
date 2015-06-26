struct VS_OUTPUT {
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

cbuffer ImageEffectConstants: register(b0) {
    vector<float, 2> RenderTargetPixelSize;
    vector<float, 2> RcpFrame;
};

cbuffer FishEyeBlock: register(b1) {
    float Strength;
};

Texture2D<float4> DiffuseTexture: register(t0);
SamplerState      TextureSampler: register(s0);

static const float PI = 3.1415926535;

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

float4 FishEyePS(VS_OUTPUT input) : SV_Target
{
    float2 uv = FishEye(input.TextureCoord.xy, RenderTargetPixelSize.xy, Strength);
    float4 color = DiffuseTexture.Sample(TextureSampler, uv);
    return color;
}
