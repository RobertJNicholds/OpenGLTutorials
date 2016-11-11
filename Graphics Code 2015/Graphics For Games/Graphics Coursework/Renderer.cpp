#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
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

	lightDirections = new LightDirection[6]
	{
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f) },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f) },
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f) },
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f) }
	};

	init = true;
}

Renderer::~Renderer(void)
{
	delete terrain;
	delete camera;
	delete terrainShader;
	delete skyboxShader;
	delete postShader;
	delete shadowShader;
	delete quad;
	delete[] lightDirections;

	currentShader = 0;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &shadowTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &shadowFBO);
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
		|| !terrain->GetSnowTex() || !cubeMap || !shadowTex)
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

	shadowShader  = new Shader(SHADERDIR"CW/shadowVertex.glsl",
							   SHADERDIR"CW/shadowFragment.glsl");


	if (!terrainShader->LinkProgram() || !postShader->LinkProgram()
		||!skyboxShader->LinkProgram() || !shadowShader->LinkProgram())
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

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glGenTextures(1, &shadowCubeMap);
	glBindTexture(GL_TEXTURE_2D, shadowCubeMap);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, 2048, 2048, 0, GL_RED, GL_FLOAT, NULL);
	}

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

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE || !shadowTex)
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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//DrawSkybox();
	DrawShadowScene();
	DrawTerrain();

	//DrawPostProcess();
	//PresentScene();
	SwapBuffers();
}

void Renderer::DrawTerrain()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetCurrentShader(terrainShader);
	SetShaderLight(*terrain->GetLight());

	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "grassTex"), 0);
	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "rockTex"), 1);
	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "shadowTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	Vector3 dirLight = Vector3(0.0f, -0.5f, 0.5f);

	glUniform3fv(glGetUniformLocation(
		currentShader->GetProgram(), "light_direction"), 1, (float*)&dirLight);
	
	Matrix4 lightMat = Matrix4::BuildViewMatrix(terrain->GetLight()->GetPosition(), terrain->GetLight()->GetPosition() - Vector3(-1, 0, 0), Vector3(0, 1, 0));

	glUniformMatrix4fv(glGetUniformLocation(
		currentShader->GetProgram(), "lightMat"), 1, false, *&lightMat.values);

	modelMatrix.ToIdentity();
	
	//textureMatrix.ToIdentity();
	Matrix4 tempMat = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
		, "textureMatrix"), 1, false, *&tempMat.values);

	viewMatrix = camera->BuildViewMatrix();
	SwitchToPerspective();

	UpdateShaderMatrices();
	terrain->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 2048, 2048);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	//modelMatrix.ToIdentity();
	
	viewMatrix = Matrix4::BuildViewMatrix(terrain->GetLight()->GetPosition(), 
		terrain->GetLight()->GetPosition() - Vector3(-1, 0, 0),
		Vector3(0, 1, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();
	terrain->Draw();
	glUseProgram(0);
	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, 1280, 720);
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
		(float)width / (float)height, 70.0f);
}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
}