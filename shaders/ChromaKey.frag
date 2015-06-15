
varying mediump vec4 texCoord;

uniform sampler2D	in_Texture;
uniform vec3		keyColor;
uniform vec3		lowColor;
uniform vec3		highColor;
uniform bool		useYCbCr;
uniform bool		useCrominance;
uniform bool		maskOutput;
uniform float		threshold;
uniform float		slope;
uniform float		alphaLowPass;
uniform float		alphaHighPass;


vec3 Rgb2YCbCr(vec3 color)
{
	return vec3((0.299 * color.r) + (0.587 * color.g) + (0.114 * color.b),
				0.5 - (0.168736 * color.r) + (0.331264 * color.g) + (0.5 * color.b),
				0.5 + (0.5 * color.r) + (0.418688 * color.g) + (0.081312 * color.b) );
}
			
vec3 RGB2YCbCr(vec3 c)
{
	return vec3(
		0.2989 * c.r + 0.5866 * c.g + 0.1145 * c.b,
		-0.1687 * c.r - 0.3313 * c.g + 0.5000 * c.b,
		0.5000 * c.r - 0.4184 * c.g - 0.0816 * c.b );
}
			
vec3 YCbCr2RGB(vec3 c)
{
	return vec3(
				c[0] + 0.0000 * c[1] + 1.4022 * c[2],
				c[0] - 0.3456 * c[1] - 0.7145 * c[2],
				c[0] + 1.7710 * c[1] + 0.0000 * c[2] );
}

			
float ChromaKeyAlpha(vec3 fg, vec3 matte)
{
	float d = abs(length(abs(matte - fg))); 
	float edge = threshold * (1.0 - slope); 
	return smoothstep(edge, threshold, d);
}

vec4 ChromaKey(vec3 fg, vec3 matte)
{
	float d = abs(length(abs(matte - fg))); 
	float edge = threshold * (1.0 - slope); 
	float alpha = smoothstep(edge, threshold, d);
	return vec4(fg, alpha);
}

void main(void)
{
    //gl_FragColor = texture(in_Texture, texCoord.st);
	//gl_FragColor = ChromaKey(fg, keyColor); //texture2D(in_Texture, texCoord.st);


	vec3 fg = RGB2YCbCr( texture(in_Texture, texCoord.st).rgb);
	//vec3 fg = texture(in_Texture, texCoord.st).rgb;
	vec3 low = lowColor; // vec3(0.321, 0.422, 0.117); 
	vec3 high = highColor; // vec3(0.545, 0.554, 0.388);
	float alpha = 1.0;


	if(useCrominance)
	{
		if( 
			fg.r > low.r && fg.r < high.r &&
			fg.g > low.g && fg.g < high.g &&
			fg.b > low.b && fg.b < high.b ) 
		{
			alpha = 0.0;
		}
		else
		{
			alpha = ChromaKeyAlpha(fg, low);
		}
	}
	else
	{
		if( fg.r > low.r && fg.r < high.r ) 
		{
			alpha = 0.0;
		}
		else
		{
			alpha = ChromaKeyAlpha(fg, low);
		}
	}

	if(alpha > alphaLowPass)
		alpha = 1.0;

	if(alpha < alphaHighPass)
		alpha = 0.0;

	if (maskOutput)
	{
		gl_FragColor =  vec4(alpha, alpha, alpha, alpha);
	}
	else
	{
		
		gl_FragColor = vec4( YCbCr2RGB(fg * alpha), 1);
	}
	
};