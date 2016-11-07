#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	wireframe = false;
	terrain = new Terrain((std::string)TEXTUREDIR"terrain.raw");
	camera = new Camera();

	LoadTextures();
	LoadShaders();

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	init = true;
}

Renderer::~Renderer(void)
{
	delete terrain;
	delete camera;
	delete terrainShader;

	currentShader = 0;
}

void Renderer::LoadTextures()
{
	terrain->SetGrassTex(SOIL_load_OGL_texture(
		TEXTUREDIR"grass.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	terrain->SetRockTex(SOIL_load_OGL_texture(
		TEXTUREDIR"rock.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	terrain->SetSnowTex(SOIL_load_OGL_texture(
		TEXTUREDIR"snow.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!terrain->GetGrassTex() || !terrain->GetRockTex()
		|| !terrain->GetSnowTex())
		return;

	SetTextureRepeating(terrain->GetGrassTex(), true);
	SetTextureRepeating(terrain->GetRockTex(), true);
}

void Renderer::LoadShaders()
{
	terrainShader = new Shader(SHADERDIR"CW/terrainVertex.glsl",
							   SHADERDIR"CW/terrainFragment.glsl");

	if (!terrainShader->LinkProgram())
		return;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	DrawTerrain();
	SwapBuffers();
}

void Renderer::DrawTerrain()
{
	SetCurrentShader(terrainShader);
	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "grassTex"), 0);
	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "rockTex"), 1);

	UpdateShaderMatrices();

	terrain->Draw();

	glUseProgram(0);
}

void Renderer::ToggleWireframe()
{
	wireframe = !wireframe;
}