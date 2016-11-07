#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");	
	camera = new Camera();
	camera->SetPosition(Vector3(907.f, 226.f, 1023.f));
	quad = Mesh::GenerateQuad();

	skyboxShader =	new Shader(SHADERDIR"Terrain/skyboxVertex.glsl",
							   SHADERDIR"Terrain/skyboxFragment.glsl");

	lightShader =	new Shader(SHADERDIR"Terrain/heightVertex.glsl",
							   SHADERDIR"Terrain/heightFragment.glsl");

	reflectShader = new Shader(SHADERDIR"Terrain/heightVertex.glsl",
							   SHADERDIR"Terrain/reflectFragment.glsl");

	sceneShader =	new Shader(SHADERDIR"texturedVertex.glsl",
							   SHADERDIR"texturedFragment.glsl");

	postShader =	new Shader(SHADERDIR"Terrain/postVertex.glsl",
					           SHADERDIR"Terrain/postFragment.glsl");

	shadowShader =	new Shader(SHADERDIR"Terrain/shadowVertex.glsl",
						       SHADERDIR"Terrain/shadowFragment.glsl");

	if (!skyboxShader->LinkProgram() || !lightShader->LinkProgram() 
		|| !reflectShader->LinkProgram() || !postShader->LinkProgram()
		|| !sceneShader->LinkProgram() || !shadowShader->LinkProgram())
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

	GenerateFramebufferTex();
	GenerateFramebuffers();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
	delete shadowShader;
	delete quad;	
	currentShader = 0;

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &shadowTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Renderer::GenerateFramebufferTex()
{
	//Depth Tex
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
				 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//Colour Textures
	for (int i = 0; i < 2; ++i)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	//Shadow Texture
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateFramebuffers()
{
	glGenFramebuffers(1, &bufferFBO); // scene buffer
	glGenFramebuffers(1, &processFBO); //post-process buffer
	glGenFramebuffers(1, &shadowFBO); //Shadow buffer

	//scene buffer
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
						   GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						   GL_TEXTURE_2D, bufferColourTex[0], 0);
	
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE || !shadowTex)
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene()
{	

	//DrawSkybox();
	DrawShadowScene();
	DrawHeightmap();
	//DrawWater();
	DrawPostProcess();
	PresentScene();
	SwapBuffers();
}

void Renderer::DrawSkybox()
{	
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);	
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	Vector3 dirlight = Vector3(-0.5f, 1.0f, -2.0f);

	SwitchToOrthographic();
	//projMatrix = Matrix4::Orthographic(-10.f, 10.f, -10.f, 10.f, -10.f, 20.0f);
	viewMatrix = Matrix4::BuildViewMatrix(dirlight, Vector3(0, 0, 0));

	Matrix4 MVP = projMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "MVP"), 1, GL_FALSE, (float*)&MVP);

	heightMap->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glUseProgram(0);
}

void Renderer::DrawHeightmap()
{		
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	

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

	Vector3 dirlight = Vector3(0.5f, -1.0f, 2.0f);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"lightDirection"), 1, (float*)&dirlight);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	SwitchToPerspective();

	UpdateShaderMatrices();
	heightMap->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawWater()
{		
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);	
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"cubeTex"), 1);

	glActiveTexture(GL_TEXTURE1);
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(postShader);
	
	SwitchToOrthographic();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	//// Now to swap the colour buffers , and do the second blur pass

	/*glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);

	quad->SetTexture(bufferColourTex[1]);
	quad->Draw();	*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneShader);
	SwitchToOrthographic();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	quad->SetTexture(bufferColourTex[0]);
	quad->Draw();
	glUseProgram(0);	
}

void Renderer::SwitchToPerspective()
{
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
}
