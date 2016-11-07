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

	void ToggleWireframe();

protected:

	void DrawTerrain();
	void DrawPostProcess();
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
	Shader* postShader;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex;
	GLuint bufferDepthTex;

	Mesh* quad;

	bool wireframe;	
};
