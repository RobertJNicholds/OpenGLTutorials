#version 150
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2DShadow shadowTex;

uniform vec3 light_direction;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;

in Vertex {		
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	float height;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColor;

void main(void){

	vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if(IN.height > 1.0f)
	{
		diffuse =  texture(rockTex, IN.texCoord);	
	}
	
	if(IN.height < 1.0f)
	{
		diffuse = texture(grassTex, IN.texCoord);
	}	

	vec3 I = normalize(lightPos - IN.worldPos);
	float lambert = max(0.0, dot(I, IN.normal));
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	float shadow = 1.0;

	if(IN.shadowProj.w > 0.0)
	{
		shadow = textureProj(shadowTex, IN.shadowProj);
	}

	lambert *= shadow;	

	vec3 colour = (diffuse.rgb * lightColour.rgb);
	
	fragColor = (vec4(0.2,0.2,0.2,1.0) * diffuse);
	fragColor += vec4(colour * atten * lambert, 1.0);

	vec3 N = normalize(IN.normal);
	vec3 L = normalize(-light_direction);
	float diffuse_intensity = max(dot(L, N), 0.0);

	//fragColor = (vec4(0.1,0.1,0.1,1.0) * diffuse) +	vec4((vec3(0.87,0.87,0.87) * diffuse_intensity),1.0) * diffuse;
}