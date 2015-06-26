#version 330

in VertexData {
    vec2 TextureCoord;
    vec4 Color;
    float TextureIndex;
} In;

#if __VERSION__ < 420
uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
#else
layout(binding=0) uniform sampler2D Texture0;
layout(binding=1) uniform sampler2D Texture1;
layout(binding=2) uniform sampler2D Texture2;
layout(binding=3) uniform sampler2D Texture3;
#endif

out vec4 FragColor;

vec4 FindTextureColor(int textureIndex, vec2 uv)
{
    if (textureIndex == 0) {
        return texture(Texture0, uv);
    }
    else if (textureIndex == 1) {
        return texture(Texture1, uv);
    }
    else if (textureIndex == 2) {
        return texture(Texture2, uv);
    }
    return texture(Texture3, uv);
}

void main()
{
    int textureIndex = clamp(int(In.TextureIndex), 0, 3);
    vec4 color = FindTextureColor(textureIndex, In.TextureCoord.xy).rgba;
    FragColor = color * In.Color.rgba;
}
