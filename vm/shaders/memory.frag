#version 140

uniform sampler2D uni_Texture;


in vec2 vertexShader_UV;

out vec4 out_FragColor;


void main(void)
{
    vec4 texColour = texture(uni_Texture, vertexShader_UV);
    out_FragColor = texColour;
}
