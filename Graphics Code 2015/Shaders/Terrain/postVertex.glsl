#version 150

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {	
	vec2 texCoord;	
	vec4 colour;
} OUT;


void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	OUT.texCoord = texCoord;
	gl_Position = mvp * vec4(position, 1.0);
}
