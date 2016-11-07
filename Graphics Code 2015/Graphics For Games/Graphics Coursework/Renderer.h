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

	void LoadTextures();
	void LoadShaders();

	Terrain* terrain;
	Camera* camera;

	Shader* terrainShader;

	bool wireframe;
};
