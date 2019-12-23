#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

// per Instance
// {xy__} = position.xy
// {__zw} = scale.xy
layout(location = 2) in vec4 Translation;
// {xyzw} = color.rgba
layout(location = 3) in vec4 Color;

uniform MyShaderConstants {
    mat4x4 ViewProjection;
};

out VertexData {
    vec2 TextureCoord;
    vec4 Color;
} Out;

void main()
{
    mat4x4 scaling = mat4x4(
        vec4(Translation.z, 0.0, 0.0, 0.0),
        vec4(0.0, Translation.w, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));

    mat4x4 translate = mat4x4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(Translation.x, Translation.y, 0.0, 1.0));

    mat4x4 transform = translate * scaling;
    vec4 worldSpacePosition = transform * vec4(Position.xyz, 1.0);
    gl_Position = ViewProjection * worldSpacePosition;
    Out.TextureCoord = TextureCoord.xy;
    Out.Color = Color;
}
