#version 150 core

in  vec3 position;
in  vec4 colour;

uniform float rot_angle;

out Vertex {
	vec4 colour;
} OUT;


mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(void)	{

	mat4 rotate = rotationMatrix(vec3(0.0f, 1.0f, 0.0f), rot_angle);
	mat4 rotate1 = rotationMatrix(vec3(0.0f, 0.0f, 1.0f), rot_angle);
	mat4 rotate2 = rotationMatrix(vec3(1.0f, 0.0f, 0.0f), rot_angle);
	gl_Position	  = vec4(position, 1.0) * rotate * rotate1 * rotate2;
	OUT.colour    = colour;
}
