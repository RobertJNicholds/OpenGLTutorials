#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bloomTex;

in Vertex 
{
	vec2 texCoord;	
} IN;

out vec4 fragColour;

void main()
{
	const float gamma = 1.0;
	vec3 colour = texture(diffuseTex, IN.texCoord).rgb;		
	vec3 bloomColour = texture(bloomTex, IN.texCoord).rgb;

	colour += bloomColour;

	vec3 result = vec3(1.0) - exp(-colour * 0.5);
	result = pow(result, vec3(1.0 / gamma));

	//vec3 mapped = colour / (colour + vec3(1.0));

	//mapped = pow(mapped, vec3(1.0 / gamma));
	fragColour = vec4(result,1.0);
}
