#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex {	
	vec2 texCoord;	
	float height;
} OUT;


void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;	
	if(position.y > 140.0f)
	{
		OUT.height = 2.0f;
	}
	else
	{
		OUT.height = 0.0f;
	}
}