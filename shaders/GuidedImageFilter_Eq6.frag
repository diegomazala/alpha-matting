#version 420

uniform sampler2D in_Texture0;			// mean p
uniform sampler2D in_Texture1;			// mean I
uniform sampler2D in_Texture2;			// ak
uniform int     radius = 3;				// The radius of window (in pixels).

out vec4 out_Color;


void main()
{
	int kernelSize = radius * 2;
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
	
	vec3 pk = texelFetch(in_Texture0, texCoord, 0).rgb;
	vec3 uk = texelFetch(in_Texture1, texCoord, 0).rgb;
	vec3 ak = texelFetch(in_Texture2, texCoord, 0).rgb;

	vec3 bk = pk - ak * uk;

	out_Color = vec4(bk, 1.0);
}