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
// {__z_} = layerDepth
// {___w} = rotation
layout(location = 2) in vec4 TransformMatrix2;
// {xy__} = xy
// {__zw} = {width, height}
layout(location = 3) in vec4 SourceRect;
// {xy__} = originPivot.xy
// {__zw} = scale.xy
layout(location = 4) in vec4 OriginScale;
// {xyzw} = color.rgba
layout(location = 5) in vec4 Color;

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

	float cosRotation = cos(TransformMatrix2.w);
	float sinRotation = sin(TransformMatrix2.w);
	mat3x3 rotate = mat3x3(
		vec3(cosRotation, -sinRotation, 0.0f),
		vec3(sinRotation, cosRotation, 0.0f),
		vec3(0.0f, 0.0f, 1.0f));

	mat3x3 localTransform = (scaling * rotate);
	vec3 position = (vec3(PositionTextureCoord.xy - OriginScale.xy, 1.0) * localTransform);
	
	mat3x3 worldMatrix = transpose(mat3x3(
		vec3(TransformMatrix1.xy, 0),
		vec3(TransformMatrix1.zw, 0),
		vec3(TransformMatrix2.xy, 1)));
	position = position * worldMatrix;

	gl_Position = vec4(position.xy, 0.0, 1.0) * ViewProjection;

	Out.TextureCoord = (PositionTextureCoord.zw * SourceRect.zw + SourceRect.xy) * InverseTextureWidth.xy;
	Out.Color = Color;

	//Out.TextureCoord = PositionTextureCoord.zw;
}
