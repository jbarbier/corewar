#version 140

uniform mat4	uni_ProjectionMatrix;
uniform vec4	uni_Color;

in	vec4		in_Position;
in  float		in_Value;

out vec4		vertexShader_Color;

void main(void) 
{
    vec4 position = vec4(in_Position.xy, 0.0, 1.0);
	float v = ceil(in_Value / 255.0);	
    vertexShader_Color = vec4(uni_Color.xyz,  clamp(v, 0.3, 0.8) * ceil(v));
	gl_Position = uni_ProjectionMatrix * position;
}
