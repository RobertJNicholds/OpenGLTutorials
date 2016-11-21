#include "TextMesh.h"

TextMesh::TextMesh(const std::string &text, const Font &font) : font(font)
{
	SetTexture(font.texture);

	type = GL_TRIANGLE_STRIP;

	numVertices = text.length() * 4;

	vertices	  = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];

	float texelWidth  = 1.0f / font.xCount;
	float texelHeight = 1.0f / font.yCount;

	for (unsigned int i = 0; i < text.length(); ++i)
	{
		unsigned int c = (unsigned int)text[i];

		float x = (float)(c%font.xCount);
		float y = (float)((c / font.xCount) % font.yCount);

		vertices[(i * 4)] = Vector3((float)i, 0, 0);
		vertices[(i * 4) + 1] = Vector3((float)i, -1, 0);
		vertices[(i * 4) + 2] = Vector3((float)i + 1, 0, 0);
		vertices[(i * 4) + 3] = Vector3((float)i + 1, -1, 0);

		textureCoords[(i * 4)] = Vector2(x*texelWidth, (y)*texelHeight);
		textureCoords[(i * 4) + 1] = Vector2(x*texelWidth, (y + 1) * texelHeight);
		textureCoords[(i * 4) + 2] = Vector2((x + 1)*texelWidth, (y)*texelHeight);
		textureCoords[(i * 4) + 3] = Vector2((x + 1)*texelWidth, (y + 1) * texelHeight);
	}

	BufferData();
}

TextMesh::~TextMesh(void) {
	texture = 0;
}