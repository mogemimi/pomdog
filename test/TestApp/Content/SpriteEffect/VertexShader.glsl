#version 330

layout(location = 0) in vec4 PositionTextureCoord;

// per Instance
layout(location = 1) in mat4x4 WorldMatrix;
// {xyz_} = position.xyz
// {___w} = rotation
layout(location = 5) in vec4 Translation;
// {xy__} = xy
// {__zw} = {width, height}
layout(location = 6) in vec4 SourceRect;
// {xy__} = originPivot.xy
// {__zw} = scale.xy
layout(location = 7) in vec4 OriginScale;
// {xyzw} = color.rgba
layout(location = 8) in vec4 Color;

out VertexData {
	vec2 TextureCoord;
	vec4 Color;
} Out;

uniform Matrices {
	mat4x4 ViewProjection;
};

uniform TextureConstants {
	vec2 InverseTextureWidth;
};

void main()
{
	mat3x3 scaling = mat3x3(
		vec3(OriginScale.z * SourceRect.z, 0.0, 0.0),
		vec3(0.0, OriginScale.w * SourceRect.w, 0.0),
		vec3(0.0, 0.0, 1.0));

	float cosRotation = cos(Translation.w);
	float sinRotation = sin(Translation.w);
	mat3x3 rotate = mat3x3(
		vec3(cosRotation, -sinRotation, 0.0f),
		vec3(sinRotation, cosRotation, 0.0f),
		vec3(0.0f, 0.0f, 1.0f));

	mat3x3 translate = mat3x3(
		vec3(1.0, 0.0, Translation.x),
		vec3(0.0, 1.0, Translation.y),
		vec3(0.0, 0.0, 1.0));

	mat3x3 localTransform = ((scaling * rotate) * translate);
	vec3 position = (vec3(PositionTextureCoord.xy - OriginScale.xy, 1.0) * localTransform);

	mat4x4 transform = WorldMatrix * ViewProjection;
	gl_Position = vec4(position.xy, 0.0, 1.0) * transform;

	Out.TextureCoord = (PositionTextureCoord.zw * SourceRect.zw + SourceRect.xy) * InverseTextureWidth.xy;
	Out.Color = Color;

	//Out.TextureCoord = PositionTextureCoord.zw;
}
