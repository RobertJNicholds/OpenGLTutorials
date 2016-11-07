#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "../../nclgl/Mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class Terrain : public Mesh
{
public:
	Terrain(std::string &filename);
	Terrain(void);
	virtual ~Terrain(void);

	virtual void Draw();

	void	SetGrassTex(GLuint tex) { grassTex = tex; }
	GLuint	GetGrassTex() { return grassTex; }

	void	SetRockTex(GLuint tex) { rockTex = tex; }
	GLuint	GetRockTex() { return rockTex; }

	void	SetSnowTex(GLuint tex) { snowTex = tex; }
	GLuint	GetSnowTex() { return snowTex; }

protected:

	GLuint grassTex;
	GLuint rockTex;
	GLuint snowTex;
};