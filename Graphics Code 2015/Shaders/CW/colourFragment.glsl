#version 150
uniform vec4 colour;

in Vertex {		
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void){
	fragColor =  colour;
}