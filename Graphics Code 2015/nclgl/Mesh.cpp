#include "Mesh.h"

Mesh::Mesh(void)
{
	for (int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	texture = 0;
	vertices = NULL;
	normals = NULL:
	indices = NULL;
	numIndices = 0;
	colours = NULL;
	textureCoords = NULL;
	type = GL_TRIANGLES;
}

Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	delete[] vertices;
	delete[] normals;
	delete[] indices;
	delete[] colours;
	delete[] textureCoords;
}

Mesh* Mesh::GenerateTriangle()
{
	Mesh* mesh = new Mesh();
	mesh->numVertices = 3;
	mesh->vertices = new Vector3[mesh->numVertices];
	mesh->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	mesh->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	mesh->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	mesh->textureCoords = new Vector2[mesh->numVertices];
	mesh->textureCoords[0] = Vector2(0.5f, 0.0f);
	mesh->textureCoords[1] = Vector2(1.0f, 1.0f);
	mesh->textureCoords[2] = Vector2(0.0f, 1.0f);

	mesh->colours = new Vector4[mesh->numVertices];
	mesh->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	mesh->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	mesh->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	mesh->BufferData();
	return mesh;
}

Mesh* Mesh::GenerateQuad()
{
	Mesh* mesh = new Mesh();
	mesh->numVertices = 4;
	mesh->type = GL_TRIANGLE_STRIP;

	mesh->vertices = new Vector3[mesh->numVertices];
	mesh->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	mesh->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	mesh->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	mesh->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	mesh->textureCoords = new Vector2[mesh->numVertices];
	mesh->textureCoords[0] = Vector2(0.0f, 1.0f);
	mesh->textureCoords[1] = Vector2(0.0f, 0.0f);
	mesh->textureCoords[2] = Vector2(1.0f, 1.0f);
	mesh->textureCoords[3] = Vector2(1.0f, 0.0f);

	mesh->colours = new Vector4[mesh->numVertices];
	for (int i = 0; i < 4; ++i)
	{
		mesh->colours[i] = Vector4(1.0f);
	}

	mesh->BufferData();
	return mesh;
}

void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);

	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
			normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	if (indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
			bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint),
			indices, GL_STATIC_DRAW);
	}

	if (textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2),
			textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4),
			colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}	

	glBindVertexArray(0);
}

void Mesh::GenerateNormals()
{
	if (!normals)
	{
		normals = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i] = Vector3();
	}
	if (indices)
	{
		for (GLuint i = 0; i < numIndices; i += 3)
		{
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross(
				(vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else
	{
		for (GLuint i = 0; i < numVertices; i += 3)
		{
			Vector3 &a = vertices[i];
			Vector3 &b = vertices[i + 1];
			Vector3 &c = vertices[i + 2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i + 1] = normal;
			normals[i + 2] = normal;
 		} 
	}
	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i].Normalise();
	}
}

void Mesh::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, snow);
	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}