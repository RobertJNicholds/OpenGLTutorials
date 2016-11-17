#version 150

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 biasMatrix;

in vec3 position;

void main(void) {
	mat4 mvp = biasMatrix * projMatrix * viewMatrix * modelMatrix;	
	gl_Position = modelMatrix * vec4(position, 1.0);
}