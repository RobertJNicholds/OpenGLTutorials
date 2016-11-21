#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJMesh.h"
#include <random>

#include "Terrain.h"
#include "TextMesh.h"
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

	void ToggleOcclusion();

protected:

	struct LightDirection
	{
		Vector3 target;
		Vector3 up;
	};

	void FillGBuffer();		
	void SSAOPass();
	void SSAOBlurPass();
	void DrawShadowScene();	
	void DrawPointLights();
	void CombineBuffers();
	void BloomBlurPass();
	void DrawPostProcess();
	void PresentScene();

	void DrawScreenText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	void GenerateFramebuffers();
	void GenerateFramebufferTextures();
	void GenerateScreenTexture(GLuint &into, bool depth = false);
	void LoadTextures();
	void LoadShaders();

	void GenerateSSAONoise();

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
	Shader* ssaoShader;
	Shader* ssaoBlurShader;
	Shader* textShader;
	Shader* colourShader;
	Shader* blurShader;
	
	std::string FPSstring;
	int FPS;
	float dT;
	int frames;
	int use_occlusion;
	
	GLuint gbuffer;
	GLuint pointLightFBO;
	GLuint lbufferFBO;
	GLuint bloomFBO;
	GLuint bloomTex[2];
	GLuint postProcessFBO;
	GLuint lbufferTex;
	GLuint brightTex; //Lbuffer attachment to store bright pixels;
	GLuint gColourTex;
	GLuint gNormalTex;
	GLuint gDepthTex;
	GLuint gPositionTex;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	GLuint ssaoFBO;
	GLuint ssaoBlurFBO;
	GLuint ssaoColourTex;
	GLuint ssaoColorBufferBlur;
	GLuint noiseTexture;

	GLuint shaderMatricesUBO;

	std::vector<Vector3> ssaoKernel;
	std::vector<Vector3> ssaoNoise;

	GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
	{
		return a + f * (b - a);
	}

	GLuint shadowFBO;
	GLuint shadowTex;	

	OBJMesh* sphere;
	Mesh* quad;
	GLuint cubeMap;

	GLuint shadowCubeMap;
	LightDirection* lightDirections;

	Font* basicFont;
};
