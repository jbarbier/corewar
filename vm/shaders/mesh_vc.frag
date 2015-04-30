#version 140

uniform vec3		uni_LightDirection;
uniform vec4		uni_Ambient;

in vec4 vertexShader_Color;
in vec3 vertexShader_Normal;

out vec4 out_FragColor;


void main(void)
{    
    out_FragColor = clamp(vertexShader_Color + uni_Ambient, vec4(0.0), vec4(1.0));
}
