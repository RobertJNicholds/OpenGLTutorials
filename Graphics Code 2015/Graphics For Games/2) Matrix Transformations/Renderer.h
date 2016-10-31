#pragma once
#include "../../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void SwitchToPerspective();
	void SwitchToOrthographic();

	inline void SetSfcale(float s) { scale = s; }

protected:

	Mesh* triangle;

	float scale;
	float rotation;
	Vector3 position;
};