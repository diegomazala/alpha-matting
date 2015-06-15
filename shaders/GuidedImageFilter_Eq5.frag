#version 420

uniform sampler2D in_Texture0;			// p
uniform sampler2D in_Texture1;			// I
uniform sampler2D in_Texture2;			// mean p
uniform sampler2D in_Texture3;			// mean I
uniform int     radius = 3;				// The radius of window (in pixels).
uniform float   eps = 0.00001;			// The regularization parameter.

out vec4 out_Color;


void main()
{
	int kernelSize = radius * 2;
	ivec2 texCoord = ivec2(gl_FragCoord.xy);

	
	vec3 uk_color = texelFetch(in_Texture3, texCoord, 0).rgb;
	float uk_gray = dot(uk_color.rgb, vec3(0.299, 0.587, 0.114));

	// Computing ak 
	vec3 sum = vec3(0.0);
	float sum_I2 = 0.0;
	for (int i = -kernelSize; i <= kernelSize; i++)
	{
		for (int j = -kernelSize; j <= kernelSize; j++)
		{
			// for numerator
			vec3 P = texelFetch(in_Texture0, texCoord + ivec2(i, j), 0).rgb;
			vec3 I = texelFetch(in_Texture1, texCoord + ivec2(i, j), 0).rgb;
			vec3 pk = texelFetch(in_Texture2, texCoord + ivec2(i, j), 0).rgb;
			vec3 uk = texelFetch(in_Texture3, texCoord + ivec2(i, j), 0).rgb;
			sum += P * I - uk * pk;

			// for denominator
			float I_gray = dot(I.rgb, vec3(0.299, 0.587, 0.114));
			sum_I2 += pow(I_gray - uk_gray, 2.0);
		}
	}

	float size = float((2 * kernelSize + 1) * (2 * kernelSize + 1));
	float var = sum_I2 / (size - 1.0f);			// the variance of I in wk in uk

	vec3 ak_numerator = sum / size;	// normalizing
	float ak_denominator = var - eps;

	vec3 ak = ak_numerator / ak_denominator;

	out_Color = vec4(ak, 1.0);
}