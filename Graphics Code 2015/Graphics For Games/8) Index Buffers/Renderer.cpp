#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	camera = new Camera();

	currentShader = new Shader(SHADERDIR"heightVertex.glsl",
							   SHADERDIR"heightFragment.glsl");

	if (!currentShader->LinkProgram())
		return;

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetSnow(SOIL_load_OGL_texture(
		TEXTUREDIR"snow.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture() | !heightMap->GetSnow())
		return;

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetSnow(), true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}

Renderer::~Renderer(void)
{
	delete heightMap;
	delete camera;
	delete light;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());	

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"snowTex"), 1);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	UpdateShaderMatrices();
	SetShaderLight(*light);

	heightMap->Draw();

	glUseProgram(0);
	SwapBuffers();
}
