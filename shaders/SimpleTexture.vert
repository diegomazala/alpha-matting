#version 420

in highp vec4 in_Vertex;
in mediump vec4 in_TexCoord;
out mediump vec4 texCoord;

void main(void)
{
	gl_Position = in_Vertex;
	texCoord = in_TexCoord;
};