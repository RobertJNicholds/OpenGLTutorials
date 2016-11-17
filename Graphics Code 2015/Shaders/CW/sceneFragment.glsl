#version 150

uniform sampler2D grassTex;
uniform sampler2D rockTex;

in Vertex {	
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	float height;
} IN;

out vec4 Diffuse;
out vec4 Normals;

void main(void)
{
	vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	if(IN.height > 1.0f)
	{
		diffuse =  texture(rockTex, IN.texCoord);	
	}
	
	if(IN.height < 1.0f)
	{
		diffuse = texture(grassTex, IN.texCoord);
	}	

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = IN.normal;

	Diffuse = diffuse;
	Normals = vec4(normal.xyz * 0.5 + 0.5, 1.0);	
}