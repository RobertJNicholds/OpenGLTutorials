#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	terrain = new Terrain((std::string)TEXTUREDIR"60kmChamonix.png");
	camera = new Camera();
	camera->SetPosition(Vector3(907.f, 226.f, 1023.f));
	quad = Mesh::GenerateQuad();

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh("../../Meshes/ico.obj"))
	{
		return;
	}

	LoadTextures();
	LoadShaders();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	lightDirections = new LightDirection[6]
	{
		{ Vector3(1.0f, 0.0f, 0.0f),	Vector3(0.0f, -1.0f, 0.0f)},
		{ Vector3(-1.0, 0.0f, 0.0f),	Vector3(0.0f, -1.0f, 0.0f)},
		{ Vector3(0.0f, 1.0f, 0.0f),	Vector3(0.0f, 0.0f, 1.0f)},
		{ Vector3(0.0f, -1.0f, 0.0f),	Vector3(0.0f, 0.0f, -1.0f)},
		{ Vector3(0.0f, 0.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f)},
		{ Vector3(0.0f, 0.0f, -1.0f),	Vector3(0.0f, -1.0f, 0.0f)}
	};

	init = true;
}

Renderer::~Renderer(void)
{
	delete terrain;
	delete camera;	
	delete shadowShader;
	delete pointLightShader;
	delete combineShader;
	delete postShader;
	delete quad;
	delete[] lightDirections;	

	currentShader = 0;	
	glDeleteTextures(1, &shadowTex);	
	glDeleteTextures(1, &gColourTex);
	glDeleteTextures(1, &gNormalTex);
	glDeleteTextures(1, &gDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	
	glDeleteFramebuffers(1, &gbuffer);
	glDeleteFramebuffers(1, &pointLightFBO);
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
	shadowShader  = new Shader(SHADERDIR"CW/shadowVertex.glsl",
							   SHADERDIR"CW/shadowFragment.glsl",
							   SHADERDIR"CW/shadowGeometry.glsl");

	sceneShader   = new Shader(SHADERDIR"CW/sceneVertex.glsl",
							   SHADERDIR"CW/sceneFragment.glsl");

	pointLightShader = new Shader(SHADERDIR"CW/pointlightVertex.glsl",
								  SHADERDIR"CW/pointlightFragment.glsl");

	combineShader = new Shader(SHADERDIR"CW/combineVertex.glsl",
							   SHADERDIR"CW/combineFragment.glsl");

	postShader = new Shader(SHADERDIR"CW/postVertex.glsl",
						    SHADERDIR"CW/postFragment.glsl");

	presentShader = new Shader(SHADERDIR"CW/postVertex.glsl",
							   SHADERDIR"CW/presentFragment.glsl");


	if (
		   !shadowShader->LinkProgram()
		|| !sceneShader->LinkProgram()
		|| !pointLightShader->LinkProgram()
		|| !combineShader->LinkProgram()
		|| !postShader->LinkProgram()
		|| !presentShader->LinkProgram())
		return;
}

void Renderer::GenerateFramebufferTextures()
{	
	//Shadow map for point light
	glGenTextures(1, &shadowCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &lbufferTex);
	glBindTexture(GL_TEXTURE_2D, lbufferTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	GenerateScreenTexture(gDepthTex, true);
	GenerateScreenTexture(gColourTex);
	GenerateScreenTexture(gNormalTex);
	GenerateScreenTexture(gPositionTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);	
}

void Renderer::GenerateScreenTexture(GLuint &into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,
		depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,
		width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateFramebuffers()
{	
	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	//Generate our geometry buffer
	glGenFramebuffers(1, &gbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, gColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, gNormalTex, 0);	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, gDepthTex, 0);	

	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
		return;

	glGenFramebuffers(1, &pointLightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
		return;

	//Generate shadow buffer
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		shadowCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE || !shadowCubeMap)
		return;

	glGenFramebuffers(1, &lbufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lbufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lbufferTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
		return;

	glGenFramebuffers(1, &postProcessFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::UpdateScene(float msec)
{
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//std::cout << camera->GetPosition() << std::endl;
}

void Renderer::UpdateLight(Vector3 pos)
{
	terrain->SetLightPosition(terrain->GetLight()->GetPosition() + pos);
}

void Renderer::RenderScene()
{		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	FillGBuffer();
	DrawShadowScene();
	DrawPointLights();		
	CombineBuffers();
	DrawPostProcess();
	PresentScene();
	
	SwapBuffers();
}

void Renderer::FillGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	

	SetCurrentShader(sceneShader);

	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "grassTex"), 0);
	glUniform1i(glGetUniformLocation(
		currentShader->GetProgram(), "rockTex"), 1);

	SwitchToPerspective();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	terrain->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights()
{
	SetCurrentShader(pointLightShader);
	SetShaderLight(*terrain->GetLight());
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"normTex"), 4);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 5);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gDepthTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gNormalTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "farPlane"), 100000.0f);

	Matrix4 worldMat = modelMatrix;
	worldMat.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "worldMat"),
		1, false, (float*)&worldMat);

	float radius = terrain->GetLight()->GetRadius();
	modelMatrix = Matrix4::Translation(terrain->GetLight()->GetPosition()) *
		Matrix4::Scale(Vector3(radius, radius, radius));
	viewMatrix = camera->BuildViewMatrix();	
	SwitchToPerspective();

	UpdateShaderMatrices();

	float dist = (terrain->GetLight()->GetPosition()).Length();
	if (dist < radius)
	{
		glCullFace(GL_FRONT);
	}
	else
	{
		glCullFace(GL_BACK);
	}

	sphere->Draw();

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	projMatrix = Matrix4::Perspective(10.0f, 100000.0f, 1.0f, 90.0f);
	modelMatrix.ToIdentity();
	//SwitchToPerspective();
	Vector3 lightPos = terrain->GetLight()->GetPosition();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "biasMatrix"), 1, false, *&biasMatrix.values);

	Matrix4 shadowTransforms[6];

	for (int i = 0; i < 6; ++i)
	{
		shadowTransforms[i] = projMatrix * Matrix4::BuildViewMatrix(lightPos, lightPos + lightDirections[i].target, lightDirections[i].up);
	}

	std::string name_start = "shadowTransforms[";

	for (int i = 0; i < 6; ++i)
	{
		std::string count = std::to_string(i);

		std::string transformName = name_start + count + "]";
		const char* transform_str = transformName.c_str();

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), transform_str),
			1, false, *&shadowTransforms[i].values);
	}

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "farPlane"), 100000.0f);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "lightPos"), 1, (float*)&lightPos);
	
	
	UpdateShaderMatrices();
	terrain->Draw();
	glUseProgram(0);
	
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, lbufferFBO);
	SetCurrentShader(combineShader);

	SwitchToOrthographic();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"emissiveTex"), 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gColourTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	quad->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lbufferTex, 0);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(postShader);
	SwitchToOrthographic();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	quad->SetTexture(lbufferTex);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lbufferTex);
	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	SetCurrentShader(presentShader);
	SwitchToOrthographic();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lbufferTex);
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

void Renderer::SwitchToLightView()
{
	camera->SetPosition(terrain->GetLight()->GetPosition());
}