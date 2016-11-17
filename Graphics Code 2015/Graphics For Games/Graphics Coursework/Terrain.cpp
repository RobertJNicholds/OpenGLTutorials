#include "Terrain.h"

Terrain::Terrain(std::string &filename)
{
	grassTex = NULL;
	rockTex = NULL;
	snowTex = NULL;	

	light = new Light(Vector3(2819.f, 600.0f, 3424.0f),
		Vector4(1.0f, 1.0f, 1.0f, 1.0f), 200000.f);

	std::vector<unsigned char> image;
	
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	colours = new Vector4[numVertices];
	
	int imgIDX = 0;
	for (int x = 0; x < RAW_WIDTH; ++x)
	{
		for (int z = 0; z < RAW_HEIGHT; ++z)
		{
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset] = Vector3(
				x * HEIGHTMAP_X,
				image[imgIDX] * HEIGHTMAP_Y,
				z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(
				x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			imgIDX += 4;
		}
	}

	numIndices = 0;
	//bool tri = true;
	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		/*int N = x * ((RAW_WIDTH - 1) + 1);
		tri = !tri;*/
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
			/*if (tri)
			{
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 2);
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 1);
				indices[numIndices++] = (N + z);
				indices[numIndices++] = (N + z + 1);
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 2);
				indices[numIndices++] = (N + z);
			}
			else
			{
				indices[numIndices++] = (N + z + 1);
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 1);
				indices[numIndices++] = (N + z);
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 2);
				indices[numIndices++] = (N + z + (RAW_WIDTH - 1) + 1);
				indices[numIndices++] = (N + z + 1);
			}

			tri = !tri;*/
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();
}

Terrain::~Terrain(void)
{
	glDeleteTextures(1, &grassTex);
	glDeleteTextures(1, &rockTex);
	glDeleteTextures(1, &snowTex);
}

void Terrain::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glBindVertexArray(arrayObject);

	glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}