#version 330

layout(location = 0) in vec4 PositionTextureCoord;

// per Instance
// {x___} = worldMatrix.M00
// {_y__} = worldMatrix.M01
// {__z_} = worldMatrix.M10
// {___w} = worldMatrix.M11
layout(location = 1) in vec4 TransformMatrix1;
// {x___} = worldMatrix.M20
// {_y__} = worldMatrix.M21
// {__zw} = originPivot.xy
layout(location = 2) in vec4 TransformMatrix2Origin;
// {xy__} = xy
// {__zw} = {width, height}
layout(location = 3) in vec4 SourceRect;
// {xyzw} = color.rgba
layout(location = 4) in vec4 Color;
// {x___} = textureIndex
// {_yzw} = unused
layout(location = 5) in vec4 TextureIndex;

out VertexData {
    vec2 TextureCoord;
    vec4 Color;
    float TextureIndex;
} Out;

uniform Matrices {
    mat4x4 ViewProjection;
};

uniform TextureConstants {
    vec4 InverseTextureWidths1;
    vec4 InverseTextureWidths2;
};

vec2 GetInverseTextureWidth(int textureIndex)
{
    if (textureIndex == 0) {
        return InverseTextureWidths1.xy;
    }
    else if (textureIndex == 1) {
        return InverseTextureWidths1.zw;
    }
    else if (textureIndex == 2) {
        return InverseTextureWidths2.xy;
    }
    return InverseTextureWidths2.zw;
}

void main()
{
    mat2x2 scaling = mat2x2(
        vec2(SourceRect.z, 0.0),
        vec2(0.0, SourceRect.w));

    vec2 position = (PositionTextureCoord.xy - TransformMatrix2Origin.zw).xy * scaling;

    mat3x2 worldMatrix = mat3x2(
        vec2(TransformMatrix1.xy),
        vec2(TransformMatrix1.zw),
        vec2(TransformMatrix2Origin.xy));
    position = (worldMatrix * vec3(position, 1.0)).xy;

    gl_Position = vec4(position.xy, 0.0, 1.0) * ViewProjection;

    int textureIndex = clamp(int(TextureIndex.x), 0, 3);
    vec2 inverseTextureWidth = GetInverseTextureWidth(textureIndex);

    Out.TextureCoord = (PositionTextureCoord.zw * SourceRect.zw + SourceRect.xy) * inverseTextureWidth;
    Out.Color = Color;
    Out.TextureIndex = textureIndex;
}
