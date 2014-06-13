#version 330

in VertexData {
	vec2 TextureCoord;
	vec4 Color;
} In;

uniform sampler2D DiffuseTexture;

out vec4 FragColor;

//const float Smoothing = 1.0/3.0;// 12pt
//const float Smoothing = 1.0/5.0;// 32pt
//const float Smoothing = 1.0/16.0;// 72pt

//const float Weight = 0.9; // 300 // lighter
//const float Weight = 0.54; // 400 // normal
//const float Weight = 0.4; // 700 // bold

uniform DistanceFieldConstants {
	float Smoothing;
	float Weight;
};

void main()
{
	float distance = texture(DiffuseTexture, In.TextureCoord.xy).a;
	float alpha = smoothstep(Weight - Smoothing, Weight + Smoothing, distance);
	FragColor = vec4(In.Color.xyz, In.Color.w * alpha);
}
