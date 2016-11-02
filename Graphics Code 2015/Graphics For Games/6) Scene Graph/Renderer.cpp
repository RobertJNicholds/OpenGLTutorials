#include "Renderer.h"

Renderer::Renderer(Window &parent) :OGLRenderer(parent)
{
	CubeRobot::CreateCube();
	camera = new Camera();

	currentShader = new Shader(SHADERDIR"sceneVertex.glsl", SHADERDIR"sceneFragment.glsl");

	if (!currentShader->LinkProgram())
		return;

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0.0f, 30.0f, 175.0f));

	for (int i = 0; i < 10; ++i)
	{
		root[i] = new SceneNode();
		root[i]->AddChild(new CubeRobot());
		Matrix4 t = root[i]->GetTransform() * Matrix4::Translation(Vector3((float)i * 100.0f, 0.0f, 0.0f));
		root[i]->SetTransform(t);		
	}

	//root[0]->RemoveChildren();
	/*root = new SceneNode();
	root->AddChild(new CubeRobot());*/

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer(void)
{
	delete *root;
	for (int i = 0; i < 10; ++i)
	{
		delete root[i];
	}

	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	for (int i = 0; i < 10; ++i)
	{
		root[i]->Update(msec);
	}
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);

	for (int i = 0; i < 10; ++i)
	{
		DrawNode(root[i]);
	}

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawNode(SceneNode* node)
{
	if (node->GetMesh())
	{
		Matrix4 transform = node->GetWorldTransform() *
			Matrix4::Scale(node->GetModelScale());
		glUniformMatrix4fv(
		glGetUniformLocation(currentShader->GetProgram(),
		"modelMatrix"), 1, false, (float*)&transform);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float*)&node->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)node->GetMesh()->GetTexture());

		node->Draw();
	}
	
	for (std::vector<SceneNode*>::const_iterator
		i = node->GetChildIteratorStart();
		i != node->GetChildeIteratorEnd(); ++i)
	{
		DrawNode(*i);
	}
}
