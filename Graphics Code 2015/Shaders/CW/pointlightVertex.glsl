#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out mat4 inverseProjView;

void main(void) 
{
	gl_Position = (projMatrix * viewMatrix * modelMatrix)
				   *vec4(position, 1.0);

	inverseProjView = inverse(projMatrix * viewMatrix);	
}