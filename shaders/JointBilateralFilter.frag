varying mediump vec4 texCoord;
uniform sampler2D in_Texture;


uniform float   rI = 0.270;             // The intensity radius (in pixels).
uniform float   rL = 1.71;              // The geometric radius (in pixels).
uniform int     windowSize = 5;         // The window size (in pixels).


float gaussian(float l, float r)
{
	return exp(-l*l / (2.0f*r*r));
}

void main()
{
	
	// Get the sizes : 
	int nWindow = windowSize / 2;
	ivec2 sz = textureSize(in_Texture, 0);
	float sx = 1.0 / (float(sz.s));
	float sy = 1.0 / (float(sz.t));
	
	vec4 outCol = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 refCol = textureLod(in_Texture, texCoord.xy, 0.0);

	float nrm = 0.0;
	// Compute the kernel : 
	for (int i = -nWindow; i <= nWindow; i++)
	{
		for (int j = -nWindow; j <= nWindow; j++)
		{
			vec4 col = textureLod(in_Texture, texCoord.xy + vec2(j*sx, i*sy), 0.0);

			float spatial = exp( -pow(length(vec2(j, i)), 2) / pow(rL,2));
			float intensity = exp(-pow(distance(col, refCol), 2) / pow(rI, 2));
			float w = spatial * intensity;
			outCol += col * w;
			nrm += w;
		}
	}
	gl_FragColor = outCol / nrm;
}