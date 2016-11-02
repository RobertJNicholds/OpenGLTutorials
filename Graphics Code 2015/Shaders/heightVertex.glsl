#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec2 texCoord;

out Vertex {	
	vec2 texCoord;	
	float height;
	vec3 worldPos;
	vec3 normal;
} OUT;


void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;	

	mat3 normalMat = transpose(inverse(mat3(modelMatrix)));

	OUT.normal = normalize(normalMat * normalize(normal));
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;

	if(position.y > 140.0f)
	{
		OUT.height = 2.0f;
	}
	else
	{
		OUT.height = 0.0f;
	}
}