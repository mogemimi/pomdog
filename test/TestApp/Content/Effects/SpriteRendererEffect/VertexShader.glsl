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
	mat2x2 scaling = mat2x2(
		vec2(OriginScale.z * SourceRect.z, 0.0),
		vec2(0.0, OriginScale.w * SourceRect.w));

	float cosRotation = cos(TransformMatrix2.w);
	float sinRotation = sin(TransformMatrix2.w);
	mat2x2 rotate = mat2x2(
		vec2(cosRotation, -sinRotation),
		vec2(sinRotation, cosRotation));

	mat2x2 localTransform = (scaling * rotate);
	vec2 position = (PositionTextureCoord.xy - OriginScale.xy).xy * localTransform;
	
	mat3x2 worldMatrix = mat3x2(
		vec2(TransformMatrix1.xy),
		vec2(TransformMatrix1.zw),
		vec2(TransformMatrix2.xy));
	position = (worldMatrix * vec3(position, 1.0)).xy;

	gl_Position = vec4(position.xy, 0.0, 1.0) * ViewProjection;

	Out.TextureCoord = (PositionTextureCoord.zw * SourceRect.zw + SourceRect.xy) * InverseTextureWidth.xy;
	Out.Color = Color;

	//Out.TextureCoord = PositionTextureCoord.zw;
}
