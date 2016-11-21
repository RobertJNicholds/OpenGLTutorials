#version 330 core
uniform sampler2D ssaoTex;

in Vertex {		
	vec2 texCoord;	
} IN;

out float FragColor;

void main(void)
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoTex, 0));
	float result = 0.0;
	for(int x = -2; x < 2; ++x)
	{
		for(int y = -2; y < 2; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoTex, IN.texCoord + offset).r;
		}
	}

	FragColor = result / (4.0 * 4.0);
}
 