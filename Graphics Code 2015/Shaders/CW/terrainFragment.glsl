#version 150
uniform sampler2D grassTex;
uniform sampler2D rockTex;

in Vertex {		
	vec2 texCoord;
	vec4 colour;
	float height;
} IN;

out vec4 fragColor;

void main(void){
	if(IN.height > 1.0f)
	{
		fragColor =  texture(rockTex, IN.texCoord);	
	}
	
	if(IN.height < 1.0f)
	{
		fragColor = texture(grassTex, IN.texCoord);
	}	
}