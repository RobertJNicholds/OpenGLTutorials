#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	camera = new Camera();
	quad = Mesh::GenerateQuad();

	skyboxShader = new Shader(SHADERDIR"Terrain/skyboxVertex.glsl",
							  SHADERDIR"Terrain/skyboxFragment.glsl");

	lightShader = new Shader(SHADERDIR"Terrain/heightVertex.glsl",
							 SHADERDIR"Terrain/heightFragment.glsl");

	reflectShader = new Shader(SHADERDIR"Terrain/heightVertex.glsl",
							   SHADERDIR"Terrain/reflectFragment.glsl");

	if (!skyboxShader->LinkProgram() || !lightShader->LinkProgram() 
		|| !reflectShader->LinkProgram())
		return;

	quad->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"!Water.bmp",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetSnow(SOIL_load_OGL_texture(
		TEXTUREDIR"snow.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.JPG", TEXTUREDIR"rusted_east.JPG",
		TEXTUREDIR"rusted_up.JPG", TEXTUREDIR"rusted_down.JPG",
		TEXTUREDIR"rusted_south.JPG", TEXTUREDIR"rusted_north.JPG",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0);

	if (!heightMap->GetTexture() || !heightMap->GetSnow() 
		|| !heightMap->GetBumpMap() || !cubeMap || !quad->GetTexture())
		return;

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetSnow(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	light = new Light;

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);	

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//glCullFace(GL_BACK);

	waterRotate = 0.0f;
	init = true;
}

Renderer::~Renderer(void)
{
	delete heightMap;
	delete camera;
	delete light;
	delete lightShader;
	delete postShader;
	delete skyboxShader;
	delete reflectShader;
	delete quad;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	SwapBuffers();
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap()
{	
	SetCurrentShader(lightShader);
	SetShaderLight(*light);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"snowTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	Vector3 dirlight = Vector3(0.0f, -0.8f, -1.0f);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"lightDirection"), 1, (float*)&dirlight);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();


	heightMap->Draw();

	glUseProgram(0);
}

void Renderer::DrawWater()
{
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH*HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 4.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	Vector3 dirlight = Vector3(0.0f, -0.8f, -1.0f);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"lightDirection"), 1, (float*)&dirlight);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));

	UpdateShaderMatrices();
	quad->Draw();
	glUseProgram(0);
}
