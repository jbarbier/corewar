#version 140

uniform mat4	uni_ProjectionMatrix;
uniform mat4	uni_LocalMatrix;
uniform vec4	uni_Diffuse;

in	vec3		in_Position;
in  vec3		in_Normal;

out vec3		vertexShader_Normal;
out vec4		vertexShader_Color;

void main(void) 
{
    vertexShader_Color = uni_Diffuse;
	vertexShader_Normal = (uni_LocalMatrix * vec4(in_Normal, 0.0)).xyz;
	gl_Position = vec4( (uni_ProjectionMatrix * uni_LocalMatrix * vec4(in_Position, 1.0)).xy, 0.0, 1.0);
}
