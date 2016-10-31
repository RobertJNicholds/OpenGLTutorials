#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Mesh.h"
#include "../../nclgl/Shader.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);
	virtual void RenderScene();

protected:
	Mesh*	triangle;
	float rot_angle;
};
