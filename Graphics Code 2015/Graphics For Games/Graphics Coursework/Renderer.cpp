#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	wireframe = false;
	terrain = new Terrain((std::string)TEXTUREDIR"fuji60km.png");
	camera = new Camera();
	camera->SetPosition(Vector3(907.f, 226.f, 1023.f));
	quad = Mesh::GenerateQuad();

	LoadTextures();
	LoadShaders();

	projMatrix = Matrix4::Perspective(1.0f, 10000000000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	init = true;
}

Renderer::~Renderer(void)
{
	delete terrain;
	delete camera;
	delete terrainShader;
	delete skyboxShader;
	delete postShader;
	delete quad;

	currentShader = 0;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
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

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"FullMoonRight.png", TEXTUREDIR"FullMoonLeft.png",
		TEXTUREDIR"FullMoonUp.png", TEXTUREDIR"FullMoonDown.png",
		TEXTUREDIR"FullMoonBack.png", TEXTUREDIR"FullMoonFront.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0);

	if (!terrain->GetGrassTex() || !terrain->GetRockTex()
		|| !terrain->GetSnowTex() || !cubeMap)
		return;

	SetTextureRepeating(terrain->GetGrassTex(), true);
	SetTextureRepeating(terrain->GetRockTex(), true);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	GenerateFramebufferTextures();
	GenerateFramebuffers();
}

void Renderer::LoadShaders()
{
	terrainShader = new Shader(SHADERDIR"CW/terrainVertex.glsl",
							   SHADERDIR"CW/terrainFragment.glsl");

	skyboxShader  = new Shader(SHADERDIR"CW/skyboxVertex.glsl",
							   SHADERDIR"CW/skyboxFragment.glsl");

	postShader	  = new Shader(SHADERDIR"CW/postVertex.glsl",
							   SHADERDIR"CW/postFragment.glsl");

	if (!terrainShader->LinkProgram() || !postShader->LinkProgram()
		||!skyboxShader->LinkProgram())
		return;
}

void Renderer::GenerateFramebufferTextures()
{
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
		0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glGenTextures(1, &bufferColourTex);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateFramebuffers()
{
	glGenFramebuffers(1, &bufferFBO); //scene buffer
	glGenFramebuffers(1, &processFBO); //post-process buffer

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
						   GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						   GL_TEXTURE_2D, bufferColourTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	DrawSkybox();
	DrawTerrain();
	
	
	//DrawPostProcess();
	//PresentScene();
	SwapBuffers();
}

void Renderer::DrawTerrain()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetCurrentShader(terrainShader);
	
	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "grassTex"), 0);
	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "rockTex"), 1);

	Vector3 dirLight = Vector3(0.0f, -0.5f, 0.5f);

	glUniform3fv(glGetUniformLocation(
		currentShader->GetProgram(), "light_direction"), 1, (float*)&dirLight);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	SwitchToPerspective();

	UpdateShaderMatrices();
	terrain->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkybox()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetCurrentShader(postShader);

	SwitchToOrthographic();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);

	quad->SetTexture(bufferColourTex);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(terrainShader);
	SwitchToOrthographic();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex);
	quad->Draw();
	glUseProgram(0);
}

void Renderer::SwitchToPerspective()
{
	projMatrix = Matrix4::Perspective(1.0f, 10000000.0f,
		(float)width / (float)height, 45.0f);
}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
}

void Renderer::ToggleWireframe()
{
	wireframe = !wireframe;
}