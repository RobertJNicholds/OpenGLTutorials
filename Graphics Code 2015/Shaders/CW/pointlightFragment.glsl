#version 150
uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform samplerCube shadowTex;
//uniform sampler2D ssaoTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform mat4 worldMat;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;
uniform float farPlane;

in mat4 inverseProjView;
out vec4 FragColor[2];


float ShadowCalculation(vec3 fragPos)
{
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(shadowTex, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = 80;
	float shadow = currentDepth - bias < closestDepth ? 1.0 : 0.2;
	
	return shadow;
}

void main(void){

	vec3 pos = vec3((gl_FragCoord.x * pixelSize.x),
					(gl_FragCoord.y * pixelSize.y), 0.0);
	
	pos.z = texture(depthTex, pos.xy).r;	

	vec3 normal = normalize(texture(normTex, pos.xy).xyz * 2.0 - 1.0);

	vec4 clip = inverseProjView * vec4(pos * 2.0 - 1.0, 1.0);
	pos = clip.xyz / clip.w;

	vec3 fragmentPos = vec3(worldMat * vec4(pos, 1.0));

	//float AmbientOcclusion = texture(ssaoTex, pos.xy).r;

	float dist = length(lightPos - pos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	if(atten == 0.0)
	{
		discard;
	}

	vec3 I = normalize(lightPos - pos);
	vec3 viewDir = normalize(cameraPos - pos);
	vec3 halfDir = normalize(I + viewDir);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 50.0);

	float lambert = clamp(dot(I, normal), 0.0, 1.0);
	float shadow = ShadowCalculation(fragmentPos);

	lambert *= shadow;

	FragColor[0] = vec4(lightColour.xyz * lambert * atten, 1.0);
	//FragColor[0] = vec4(vec3(0.3 * AmbientOcclusion), 1.0);
}