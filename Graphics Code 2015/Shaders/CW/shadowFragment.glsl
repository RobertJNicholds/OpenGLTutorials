#version 150 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main(void)
{
	float lightDistance = length(FragPos.xyz - lightPos);
	lightDistance = lightDistance / farPlane;
	gl_FragDepth = lightDistance;
}