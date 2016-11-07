#pragma once
#include "OGLRenderer.h"

enum MeshBuffer
{
	VERTEX_BUFFER, COLOUR_BUFFER,
	TEXTURE_BUFFER, TANGENT_BUFFER,
	NORMAL_BUFFER,	INDEX_BUFFER,	
	MAX_BUFFER
};

class Mesh
{
public:

	Mesh(void);
	virtual ~Mesh(void);

	virtual void	Draw();
	static Mesh*	GenerateTriangle();
	static Mesh*	GenerateQuad();


	void	SetTexture(GLuint tex) { texture = tex; }
	void	SetSnow(GLuint tex){ snow = tex; }
	GLuint	GetTexture() { return texture; }
	GLuint	GetSnow() { return snow; }
	void	SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint	GetBumpMap() { return bumpTexture; }

protected:
	
	void BufferData();
	void GenerateNormals();
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b,
							const Vector3 &c, const Vector2 &ta,
							const Vector2 &tb, const Vector2 &tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint numIndices;
	unsigned int* indices;
	GLuint type;

	Vector3* vertices;
	Vector3* normals;
	Vector3* tangents;
	Vector4* colours;
	Vector2* textureCoords;

	GLuint texture;
	GLuint bumpTexture;
	GLuint snow;
};
