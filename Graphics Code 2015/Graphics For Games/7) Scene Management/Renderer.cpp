#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	CubeRobot::CreateCube();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
				(float)width / (float)height, 45.0f);

	camera = new Camera();
	camera->SetPosition(Vector3(0.0f, 100.0f, 750.0f));

	currentShader = new Shader(SHADERDIR"sceneVertex.glsl", 
							   SHADERDIR"sceneFragment.glsl");

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(
		"../../Textures/stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!currentShader->LinkProgram())
		return;

	if (!quad->GetTexture())
		return;

	root = new SceneNode();

	for (int i = 0; i < 5; ++i)
	{
		SceneNode * s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(
			Vector3(0.0f, 100.0f, -300.0f + 100.0f + 100.0f * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(quad);
		root->AddChild(s);
	}

	root->AddChild(new CubeRobot());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer::~Renderer(void)
{
	delete root;
	delete quad;
	delete camera;
	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(msec);
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));;

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (std::vector<SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildeIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{	
	std::sort(transparentNodeList.begin(),
			  transparentNodeList.end(),
			  SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
			  nodeList.end(),
			  SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes()
{
	for (std::vector<SceneNode*>::const_iterator i = nodeList.begin();
	i != nodeList.end(); ++i)
	{
		DrawNode((*i));
	}

	for (std::vector<SceneNode*>::const_reverse_iterator i =
		transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i)
	{
		DrawNode((*i));
	}
}

void Renderer::DrawNode(SceneNode* node)
{
	if (node->GetMesh())
	{
		glUniformMatrix4fv(glGetUniformLocation(
			currentShader->GetProgram(), "modelMatrix"), 1, false,
			(float*)&(node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale())));

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float*)&node->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)node->GetMesh()->GetTexture());

		node->Draw();
	}
}

void Renderer::RenderScene()
{
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseText"), 0);

	DrawNodes();

	glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}