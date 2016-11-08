#version 150
uniform sampler2D grassTex;
uniform sampler2D rockTex;

uniform vec3 light_direction;

in Vertex {		
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	float height;
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

	vec3 N = normalize(IN.normal);
	vec3 L = normalize(-light_direction);
	float diffuse_intensity = max(dot(L, N), 0.0);

	fragColor = (vec4(0.1,0.1,0.1,1.0) * diffuse) +	vec4((vec3(0.87,0.87,0.87) * diffuse_intensity),1.0) * diffuse;
}