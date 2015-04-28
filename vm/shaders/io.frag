#version 140

uniform sampler2D uni_Texture;


in vec4 vertexShader_Color;
out vec4 out_FragColor;


void main(void)
{    
    out_FragColor = vertexShader_Color;
}
