#version 420

in mediump vec4 texCoord;
uniform sampler2D in_Texture;

void main(void)
{
	gl_FragColor = texture2D(in_Texture, texCoord.st);
};