#version 420

in mediump vec4 texCoord;
uniform sampler2D in_Texture;
uniform float   intensityRadius = 0.10;             // The intensity radius (in pixels).
uniform float   spatialRadius = 3.0;              // The geometric radius (in pixels).
uniform int     kernelSize = 7;         // The window size (in pixels).
out vec4 out_Color;

float gaussian(float l, float r)
{
	return exp(-l*l / (2.0f*r*r));
}

void main()
{
	// Get the sizes : 
	int half_kernel = kernelSize / 2;
	ivec2 sz = textureSize(in_Texture, 0);
	float sx = 1.0 / (float(sz.s));
	float sy = 1.0 / (float(sz.t));
	
	vec4 outCol = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 refCol = textureLod(in_Texture, texCoord.xy, 0.0);

	float nrm = 0.0;
	// Compute the kernel : 
	for (int i = -half_kernel; i <= half_kernel; i++)
	{
		for (int j = -half_kernel; j <= half_kernel; j++)
		{
			vec4 col = textureLod(in_Texture, texCoord.xy + vec2(j*sx, i*sy), 0.0);

			/*float  a = gaussian(distance(col, refCol), intensityRadius);
			float  b = gaussian(length(vec2(j, i)), spatialRadius);
			outCol += col * a * b;
			nrm += a * b;*/

			// wiki bilateral filtering
			float spatial = length(vec2(j, i)) / spatialRadius;
			float intensity = distance(col, refCol) / intensityRadius;
			float w = exp(-intensity - spatial);
			outCol += col * w;
			nrm += w;
		}
	}
	out_Color = outCol / nrm;
}