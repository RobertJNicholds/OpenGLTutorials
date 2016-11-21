#version 150
uniform sampler2D diffuseTex;

in Vertex {		
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void){
	fragColor =  vec4(1.0f, 0.0f, 1.0f, 1.0f) * texture(diffuseTex, IN.texCoord);
}