#pragma once
#include "../../nclgl/Mesh.h"
#include <vector>

struct Font
{
	GLuint texture;
	int xCount;
	int yCount;

	Font(GLuint tex, unsigned int xCount, unsigned int yCount)
	{
		this->texture = tex;
		this->xCount = xCount;
		this->yCount = yCount;
	}

	~Font()
	{
		glDeleteTextures(1, &texture);
	}
};

class TextMesh : public Mesh
{
public:
	TextMesh(const std::string &text, const Font &font);
	~TextMesh(void);

protected:

	const Font& font;
};