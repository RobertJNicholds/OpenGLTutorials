#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJMesh.h"

#include "Terrain.h"
#define SHADOWSIZE 4096

class Renderer : public OGLRenderer
{
public:

	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void UpdateLight(Vector3 pos);
	void SwitchToLightView();

protected:

	struct LightDirection
	{
		Vector3 target;
		Vector3 up;
	};

	void FillGBuffer();		
	void DrawShadowScene();	
	void DrawPointLights();
	void CombineBuffers();
	void DrawPostProcess();
	void PresentScene();

	void GenerateFramebuffers();
	void GenerateFramebufferTextures();
	void GenerateScreenTexture(GLuint &into, bool depth = false);
	void LoadTextures();
	void LoadShaders();

	void SwitchToPerspective();
	void SwitchToOrthographic();

	Terrain* terrain;
	Camera* camera;
	
	Shader* skyboxShader;	
	Shader* shadowShader;	
	Shader* sceneShader;
	Shader* pointLightShader;
	Shader* postShader;
	Shader* combineShader;
	Shader* presentShader;
	
	GLuint gbuffer;
	GLuint pointLightFBO;
	GLuint lbufferFBO;
	GLuint postProcessFBO;
	GLuint lbufferTex;
	GLuint gColourTex;
	GLuint gNormalTex;
	GLuint gDepthTex;
	GLuint gPositionTex;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	GLuint ssaoFBO;
	GLuint ssaoColourTex;
	GLuint noiseTexture;

	std::vector<Vector3> ssaoNoise;

	GLuint shadowFBO;
	GLuint shadowTex;	

	OBJMesh* sphere;
	Mesh* quad;
	GLuint cubeMap;

	GLuint shadowCubeMap;
	LightDirection* lightDirections;
};
