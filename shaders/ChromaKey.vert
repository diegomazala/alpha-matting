attribute highp vec4 in_Vertex;
attribute mediump vec4 in_TexCoord;
varying mediump vec4 texCoord;

void main(void)
{
    gl_Position = in_Vertex;
    texCoord = in_TexCoord;
};