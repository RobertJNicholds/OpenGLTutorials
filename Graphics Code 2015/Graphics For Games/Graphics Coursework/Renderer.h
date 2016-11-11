#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"

#include "Terrain.h"

class Renderer : public OGLRenderer
{
public:

	Renderer(Window &parent);
	virtual ~Renderer(void);
	
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:

	struct LightDirection
	{
		GLenum cubeMapFace;
		Vector3 target;
		Vector3 up;
	};

	LightDirection* lightDirections;

	void DrawTerrain();
	void DrawSkybox();
	void DrawPostProcess();
	void DrawShadowScene();
	void PresentScene();

	void GenerateFramebuffers();
	void GenerateFramebufferTextures();
	void LoadTextures();
	void LoadShaders();

	void SwitchToPerspective();
	void SwitchToOrthographic();

	Terrain* terrain;
	Camera* camera;

	Shader* terrainShader;
	Shader* skyboxShader;
	Shader* postShader;
	Shader* shadowShader;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint shadowFBO;
	GLuint shadowCubeMap;
	GLuint shadowTex;
	GLuint bufferColourTex;
	GLuint bufferDepthTex;

	Mesh* quad;
	GLuint cubeMap;
};
