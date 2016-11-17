#version 150

uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;

in Vertex {	
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void)
{
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(emissiveTex, IN.texCoord).xyz;

	FragColor.xyz = diffuse * 0.2;
	FragColor.xyz += diffuse * light;
	FragColor.a = 1.0;
}