#version 330

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

uniform ImageEffectConstants {
    vec2 RenderTargetPixelSize;
    vec2 RcpFrame;
};

out VertexData {
    vec4 TextureCoord;
} Out;

void main()
{
    gl_Position = vec4(Position.xyz, 1.0);

    //const float FxaaSubpixShift = 1.0/4.0;
    const float FxaaSubpixShift = 0.25;
    Out.TextureCoord = vec4(TextureCoord.xy,
        TextureCoord.xy - (RcpFrame.xy * (0.5 + FxaaSubpixShift)));
}
