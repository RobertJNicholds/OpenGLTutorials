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

	mesh->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	mesh->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	mesh->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	mesh->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	mesh->textureCoords[0] = Vector2(0.0f, 1.0f);
	mesh->textureCoords[1] = Vector2(0.0f, 0.0f);
	mesh->textureCoords[2] = Vector2(1.0f, 1.0f);
	mesh->textureCoords[3] = Vector2(1.0f, 0.0f);

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

void Mesh::Draw()
{
	/*glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	Vector4* colour = (Vector4*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	colour[0] = Vector4((float)(rand() % 2), (float)(rand() % 2), (float)(rand() % 2), 1.0f);
	colour[1] = Vector4((float)(rand() % 2), (float)(rand() % 2), (float)(rand() % 2), 1.0f);
	colour[2] = Vector4((float)(rand() % 2), (float)(rand() % 2), (float)(rand() % 2), 1.0f);
	glUnmapBuffer(GL_ARRAY_BUFFER);*/

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(arrayObject);
	glDrawArrays(type, 0, numVertices);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}