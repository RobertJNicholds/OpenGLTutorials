#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	triangle = Mesh::GenerateTriangle();
	rot_angle = 0.0f;
	currentShader = new Shader(SHADERDIR"basicVertex.glsl",
		SHADERDIR"colourFragment.glsl");

	if(!currentShader->LinkProgram()) {
		return;
	}

	init = true;
}
Renderer::~Renderer(void)	{
	delete triangle;
}

void Renderer::RenderScene()	{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	glUseProgram(currentShader->GetProgram());

	GLuint rot_id = glGetUniformLocation(currentShader->GetProgram(), "rot_angle");
	glUniform1f(rot_id, rot_angle);
	rot_angle += 0.01f;

	triangle->Draw();
	glUseProgram(0);

	SwapBuffers();	
}

