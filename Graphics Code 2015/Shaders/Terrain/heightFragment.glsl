#version 150
uniform sampler2D diffuseTex;
uniform sampler2D snowTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec3 lightDirection;

in Vertex {		
	vec2 texCoord;	
	float height;
	vec3 worldPos;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
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

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 N = normalize(TBN * (texture(bumpTex,
	IN.texCoord).rgb * 2.0 - 1.0));
	
	vec3 I = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(I, N));
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0f - clamp(dist/lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(I + viewDir);

	float rFactor = max(0.0, dot(halfDir, N));
	float sFactor = pow(rFactor, 50.0);

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	colour += (lightColour.rgb * sFactor) * 0.33;

	fragColor = vec4(colour * atten * lambert, diffuse.a);
	fragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;	

	vec3 L = normalize(-lightDirection);
	//vec3 N = normalize(IN.normal);

	float diffuse_ = max(dot(L, N), 0.0);
	//Ambient Light  + directional
	fragColor = (vec4(0.1,0.1,0.1,1.0) * diffuse) + vec4((vec3(0.87,0.87,0.87) * diffuse_),1.0) * diffuse;	
}