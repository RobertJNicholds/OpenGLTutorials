#version 150

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
//uniform sampler2D ssaoTex;

//uniform int use_occlusion;

in Vertex {	
	vec2 texCoord;
} IN;

out vec4 FragColor[2];


void main(void)
{
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(emissiveTex, IN.texCoord).xyz;
	//float AmbientOcclusion = texture(ssaoTex, IN.texCoord).r;

	
	//FragColor.xyz = vec3(1.0 * AmbientOcclusion);
	FragColor[0].xyz = diffuse * 0.2;
	
	FragColor[0].xyz += diffuse * light;
	
	FragColor[0].a = 1.0;

	//FragColor = vec4(vec3(AmbientOcclusion), 1.0);

	float brightness = dot(FragColor[0].rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
	{
		FragColor[1] = vec4(FragColor[0].rgb, 1.0);
	}
	else
	{
		FragColor[1] = vec4(0.0, 0.0, 0.0, 1.0);
	}
}