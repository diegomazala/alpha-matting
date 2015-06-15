#version 420

uniform sampler2D in_Texture0;			// I
uniform sampler2D in_Texture1;			// a
uniform sampler2D in_Texture2;			// b
uniform int     radius = 3;				// The radius of window (in pixels).
out vec4 out_Color;


void main()
{
	int kernelSize = radius * 2;
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
	
	vec3 I = texelFetch(in_Texture0, texCoord, 0).rgb;

	// Computing mean
	vec3 sum_a = vec3(0.0);
	vec3 sum_b = vec3(0.0);
	for (int i = -kernelSize; i <= kernelSize; i++)
	{
		for (int j = -kernelSize; j <= kernelSize; j++)
		{
			sum_a += texelFetch(in_Texture1, texCoord + ivec2(i, j), 0).rgb;
			sum_b += texelFetch(in_Texture2, texCoord + ivec2(i, j), 0).rgb;
		}
	}
	vec3 mean_a = sum_a / float((2 * kernelSize + 1) * (2 * kernelSize + 1));	// normalizing
	vec3 mean_b = sum_b / float((2 * kernelSize + 1) * (2 * kernelSize + 1));	// normalizing


	vec3 q = mean_a * I + mean_b;

	out_Color = vec4(q, 1.0);

}