#version 330

layout(location = 0) in vec4 PositionTextureCoord;

// per Instance
// {xy__} = position.xy
// {__zw} = scale.xy
layout(location = 1) in vec4 Translation;
// {xy__} = {rect.xy}
// {__zw} = {rect.width, rect.height}
layout(location = 2) in vec4 SourceRect;
// {xy__} = originPivot.xy
// {__z_} = rotation
// {___w} = layerDepth
layout(location = 3) in vec4 OriginRotationDepth;
// {xyzw} = color.rgba
layout(location = 4) in vec4 Color;
// {xy__} = {1.0f / textureWidth, 1.0f / textureHeight}
// {__z_} = RGBA channel flags (8-bits)
// {___w} = unused
layout(location = 5) in vec4 InverseTextureSize;

out VertexData {
    vec4 Color;

    // {x___} = RGB blend factor
    // {_y__} = Alpha blend factor
    // {__z_} = RGB compensation factor
    // {___w} = Alpha compensation factor
    vec4 BlendFactor;

    vec2 TextureCoord;
} Out;

uniform SpriteBatchConstants {
    mat4x4 ViewProjection;

    // {x___} = Smoothing
    // {_y__} = Weight
    // {__zw} = unused
    vec4 DistanceFieldParameters;
};

void main()
{
    mat3x3 scaling = mat3x3(
        vec3(Translation.z * SourceRect.z, 0.0, 0.0),
        vec3(0.0, Translation.w * SourceRect.w, 0.0),
        vec3(0.0, 0.0, 1.0));

    float cosRotation = cos(OriginRotationDepth.z);
    float sinRotation = sin(OriginRotationDepth.z);
    mat3x3 rotate = mat3x3(
        vec3(cosRotation, -sinRotation, 0.0),
        vec3(sinRotation, cosRotation, 0.0),
        vec3(0.0f, 0.0f, 1.0));

    mat3x3 translate = mat3x3(
        vec3(1.0, 0.0, Translation.x),
        vec3(0.0, 1.0, Translation.y),
        vec3(0.0, 0.0, 1.0));

    mat3x3 transform = (scaling * rotate) * translate;
    vec3 position = vec3(PositionTextureCoord.xy - OriginRotationDepth.xy, 1.0) * transform;

    // NOTE: 'ViewProjection' has already been transposed.
    vec4 finalPosition = vec4(position.xy, 0.0, 1.0) * ViewProjection;

    float channelFlags = InverseTextureSize.z;
    bool sourceRGBEnabled = mod(channelFlags, 2) == 1;
    bool sourceAlphaEnabled = mod(channelFlags, 4) >= 2;
    bool compensationRGB = mod(channelFlags, 8) >= 4;
    bool compensationAlpha = mod(channelFlags, 16) >= 8;

    vec4 blendFactor = vec4(0.0, 0.0, 0.0, 0.0);
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

    gl_Position = vec4(finalPosition.xy, OriginRotationDepth.w, 1.0);

    Out.TextureCoord = (PositionTextureCoord.zw * SourceRect.zw + SourceRect.xy) * InverseTextureSize.xy;
    Out.Color = Color;
    Out.BlendFactor = blendFactor;
}
