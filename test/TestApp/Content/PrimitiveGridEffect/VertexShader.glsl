#version 330

layout(location = 0) in vec2 Position;

out VertexData {
	vec3 DestinationColor;
} Out;

uniform GridLayout {
	mat4 ViewProjection;
	vec4 LineColor;
};

void main()
{
	gl_Position = vec4(Position.xy, 0.0, 1.0) * ViewProjection;
	Out.DestinationColor = LineColor.xyz;
}
