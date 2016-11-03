#version 150
uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;
uniform vec3 cameraPos;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec3 lightDirection;

in Vertex
{
	vec2 texCoord;	
	float height;
	vec3 worldPos;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} IN;

out vec4 fragColor;

void main ( void ) {
	vec4 diffuse = texture ( diffuseTex , IN.texCoord );
	vec3 incident = normalize ( IN.worldPos - cameraPos );
	float dist = length ( lightPos - IN.worldPos );
	float atten = 1.0 - clamp ( dist / 3000 , 0.2 , 1.0);
	vec4 reflection = texture ( cubeTex ,
	reflect ( incident , normalize ( IN.normal )));

	fragColor = ( lightColour * diffuse * atten )*( diffuse + reflection );
}