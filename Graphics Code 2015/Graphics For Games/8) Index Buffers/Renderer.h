#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "HeightMap.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);	

protected:

	void DrawHeightmap();
	void DrawSkybox();
	void DrawWater();
	void DrawPostProcess();
	void PresentScene();

	void GenerateFramebuffers();
	void GenerateFramebufferTex();

	void SwitchToPerspective();
	void SwitchToOrthographic();

	HeightMap * heightMap;
	Camera* camera;
	Light* light;
	Mesh* quad;

	Shader* skyboxShader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* postShader;
	Shader* sceneShader;

	GLuint cubeMap;
	float waterRotate;

	//FrameBuffers
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
};