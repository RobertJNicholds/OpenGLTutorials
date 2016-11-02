#version 150
uniform sampler2D diffuseTex;
uniform sampler2D snowTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {		
	vec2 texCoord;	
	float height;
	vec3 worldPos;
	vec3 normal;
} IN;

out vec4 fragColor;

void main(void){

vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if(IN.height > 1.0f)
	{
		diffuse =  texture(snowTex, IN.texCoord);	
	}
	
	if(IN.height < 1.0f)
	{
		diffuse = texture(diffuseTex, IN.texCoord);
	}	
	
	vec3 I = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(I, IN.normal));
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0f - clamp(dist/lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(I + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	float sFactor = pow(rFactor, 50.0);

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;

	fragColor = vec4(colour * atten * lambert, diffuse.a);
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;
}