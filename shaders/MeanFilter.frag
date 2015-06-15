#version 420

uniform sampler2D in_Texture0;
uniform int     radius = 3;				// The radius of window (in pixels).

out vec4 out_Color;


void main()
{
	int kernelSize = radius * 2;
	ivec2 texCoord = ivec2(gl_FragCoord.xy);

	// Computing mean
	vec3 sum_p = vec3(0.0);
	for (int i = -kernelSize; i <= kernelSize; i++)
		for (int j = -kernelSize; j <= kernelSize; j++)
			sum_p += texelFetch(in_Texture0, texCoord + ivec2(i, j), 0).rgb;

	vec3 mean = sum_p / float((2 * kernelSize + 1) * (2 * kernelSize + 1));	// normalizing
	out_Color = vec4(mean, 1.0);
}