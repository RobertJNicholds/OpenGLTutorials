#version 150 core

uniform sampler2D diffuseTex;

in Vertex 
{
	vec2 texCoord;	
} IN;

out vec4 fragColour;

void main()
{
	vec4 colour = texture(diffuseTex, IN.texCoord);		
	fragColour = colour;
}
