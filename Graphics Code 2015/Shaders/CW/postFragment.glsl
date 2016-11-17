#version 150 core

uniform sampler2D diffuseTex;

in Vertex 
{
	vec2 texCoord;	
} IN;

out vec4 fragColour;

float threshold(in float thr1, in float thr2, in float val)
{
	if(val < thr1) { return 0.0; }
	if(val > thr2) { return 1.0; }
	return val;
}

//averaged pixel intensity from 3 colour channels
float avg_intensity(in vec4 pix)
{
	return (pix.r + pix.g + pix.b)/3.0;
}

vec4 get_pixel(in vec2 coords, in float dx, in float dy)
{
	return texture2D(diffuseTex, coords + vec2(dx, dy));
}

// returns pixel color
float IsEdge(in vec2 coords)
{
  float dxtex = 1.0 / 512.0 /*image width*/;
  float dytex = 1.0 / 512.0 /*image height*/;
  float pix[9];
  int k = -1;
  float delta;

  // read neighboring pixel intensities
  for (int i=-1; i<2; i++) {
   for(int j=-1; j<2; j++) {
    k++;
    pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex,
                                          float(j)*dytex));
   }
  }

  // average color differences around neighboring pixels
  delta = (abs(pix[1]-pix[7])+
          abs(pix[5]-pix[3]) +
          abs(pix[0]-pix[8])+
          abs(pix[2]-pix[6])
           )/4.;

  return threshold(0.1,0.3,clamp(1.8*delta,0.0,1.0));
}

void main()
{
	vec4 colour = texture(diffuseTex, IN.texCoord);	
	if(IsEdge(IN.texCoord.xy) == 1)
	{
		colour.r = 1.0;
	}
	
	fragColour = colour;
}
