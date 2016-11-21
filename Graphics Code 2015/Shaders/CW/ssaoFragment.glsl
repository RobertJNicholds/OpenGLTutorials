#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D noiseTex;

uniform mat4 worldMat;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec2 pixelSize;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 1.0;

const vec2 noiseScale = vec2(1280.0/4.0, 720/4.0);

in Vertex {		
	vec2 texCoord;	
} IN;

out float FragColor;

void main(void)
{
	mat4 inverseProjView = inverse(projMatrix * viewMatrix);	

	vec3 pos = vec3((gl_FragCoord.x * pixelSize.x),
					(gl_FragCoord.y * pixelSize.y), 0.0);
	
	pos.z = texture(depthTex, pos.xy).r;	

	vec3 normal = normalize(texture(normTex, pos.xy).xyz * 2.0 - 1.0);

	vec4 clip = inverseProjView * vec4(pos * 2.0 - 1.0, 1.0);
	pos = clip.xyz / clip.w;

	vec3 fragPos = vec3(worldMat * vec4(pos, 1.0));

	vec3 randomVec = texture(noiseTex, IN.texCoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		vec3 _sample = TBN  * samples[i];
		_sample = fragPos + _sample * radius;

		vec4 offset = vec4(_sample, 1.0);
		offset = projMatrix * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = -texture(depthTex, offset.xy).r;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= _sample.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - ( occlusion / kernelSize);

	FragColor = occlusion;
}
 