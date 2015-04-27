#version 140

uniform mat4	uni_ProjectionMatrix;
uniform vec2	uni_Coord[4];

in	vec4		in_Position;
in  float		in_Value;
out vec2		vertexShader_UV;

void main(void) 
{
    vec4 position = in_Position;
	position.w = 1;
	vertexShader_UV = uni_Coord[int(in_Position.w)];
	vertexShader_UV.x = vertexShader_UV.x + in_Value;
    gl_Position = uni_ProjectionMatrix * position;
}
