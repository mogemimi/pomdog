#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

uniform MyShaderConstants {
    mat4x4 Model;
    mat4x4 ViewProjection;
};

out VertexData {
    vec2 TextureCoord;
} Out;

void main()
{
    gl_Position = (vec4(Position.xyz, 1.0) * Model) * ViewProjection;
    Out.TextureCoord = TextureCoord.xy;
}
