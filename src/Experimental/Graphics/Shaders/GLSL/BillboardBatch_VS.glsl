#version 330

// {xy__} = position.xy
// {__zw} = textureCoord.xy
layout(location = 0) in vec4 PositionTextureCoord;

// per Instance
// {xyz_} = position.xyz
// {___w} = rotation.z
layout(location = 1) in vec4 Translation;
// {xy__} = textureCoord.xy
// {__zw} = textureSize.xy
layout(location = 2) in vec4 TextureCoordRect;
// {xy__} = originPivot.xy
// {__zw} = scale.xy
layout(location = 3) in vec4 OriginPivotScale;
// {xyz_} = color.rgb
// {___w} = color.a
layout(location = 4) in vec4 Color;

out VertexData {
    vec4 Color;
    vec2 TextureCoord;
} Out;

uniform WorldConstants {
    mat4x4 ViewProjection;
    mat4x4 View;
    mat4x4 Projection;
    mat4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    vec4 LightDirection;
};

void main()
{
    vec2 scale = OriginPivotScale.zw;
    mat4x4 scaling = mat4x4(
        vec4(scale.x, 0.0, 0.0, 0.0),
        vec4(0.0, scale.y, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));

    float rotationZ = Translation.w;
    float cosRotation = cos(rotationZ);
    float sinRotation = sin(rotationZ);
    mat4x4 rotate = mat4x4(
        vec4(cosRotation, sinRotation, 0.0, 0.0),
        vec4(-sinRotation, cosRotation, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));

    mat4x4 transform = rotate * scaling;
    vec4 worldSpacePosition = transform * vec4(PositionTextureCoord.xy - OriginPivotScale.xy, 0.0, 1.0);
    vec4 viewSpaceOffset = View * vec4(Translation.xyz, 1.0);

    gl_Position = Projection * vec4(worldSpacePosition.xyz + viewSpaceOffset.xyz, 1.0);
    Out.TextureCoord = PositionTextureCoord.zw * TextureCoordRect.zw + TextureCoordRect.xy;
    Out.Color = Color;
}
