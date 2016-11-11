#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 lightMat;

in vec3 position;
in vec4 colour;
in vec2 texCoord;
in vec3 normal;

out Vertex {	
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	float height;
	vec3 worldPos;
	vec4 shadowProj;
} OUT;

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = texCoord,
	OUT.colour = colour;
	OUT.normal = normal;
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;
	OUT.shadowProj =  ( textureMatrix * vec4 ( position +( normal *1.5) ,1));

	if(position.y > 8800.0f)
	{
		OUT.height = 2.0f;
	}
	else
	{
		OUT.height = 0.0f;
	}
}