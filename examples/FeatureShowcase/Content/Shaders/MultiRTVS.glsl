#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TextureCoord;

uniform ModelConstantBuffer {
    mat4x4 Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    vec4 Material;

    // {xyzw} = color.rgba
    vec4 DiffuseColor;
};

uniform WorldConstantBuffer {
    mat4x4 ViewProjection;
    mat4x4 View;
    mat4x4 Projection;
    mat4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    vec4 LightDirection;
};

out VertexData {
    vec3 WorldSpacePosition;
    vec3 Normal;
    vec2 TextureCoord;
    vec2 DepthZW;
} Out;

void main()
{
    vec4 worldSpacePosition = Model * vec4(Position.xyz, 1.0);

    vec4 position = ViewProjection * worldSpacePosition;

    gl_Position = position;
    Out.WorldSpacePosition = worldSpacePosition.xyz;
    Out.Normal = mat3x3(Model) * Normal.xyz;
    Out.TextureCoord = TextureCoord.xy;
    Out.DepthZW = position.zw;
}
