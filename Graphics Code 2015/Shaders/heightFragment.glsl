#version 150
uniform sampler2D diffuseTex;
uniform sampler2D snowTex;

in Vertex {		
	vec2 texCoord;	
	float height;
} IN;

out vec4 fragColor;

void main(void){

	if(IN.height > 1.0f)
	{
		fragColor =  texture(snowTex, IN.texCoord);	
	}
	
	if(IN.height < 1.0f)
	{
		fragColor = texture(diffuseTex, IN.texCoord);
	}	
}