#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();

	currentShader = new Shader("MatrixVertex.glsl", "../Shaders/colourFragment.glsl");

	if (!currentShader->LinkProgram())
	{
		return;
	}

	init = true;

	SwitchToOrthographic();
}

Renderer::~Renderer()
{
	delete triangle;
}

void Renderer::SwitchToPerspective()
{
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
		width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}