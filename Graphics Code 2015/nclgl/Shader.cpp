#include "Shader.h"

Shader::Shader(std::string vFile, std::string fFile, std::string gFile)
{
	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vFile, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fFile, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!gFile.empty())
	{
		objects[SHADER_GEOMETRY] = GenerateShader(gFile,
			GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	SetDefaultAttributes();
}

Shader::~Shader(void)
{
	for (int i = 0; i < 3; ++i)
	{
		glDetachShader(program, objects[i]);
		glDeleteShader(objects[i]);
	}

	glDeleteProgram(program);
}

GLuint Shader::GenerateShader(std::string from, GLenum type)
{
	std::cout << "Compiling Shader..." << std::endl;

	std::string load;
	if (!LoadShaderFile(from, load))
	{
		std::cout << "Compiling failed!" << std::endl;
		loadFailed = true;
		return 0;
	}

	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		std::cout << "Compiling failed!" << std::endl;
		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);
		std::cout << error;
		loadFailed = true;
		return 0;
	}

	std::cout << "Compiling Success!" << std::endl << std::endl;
	loadFailed = false;
	return shader;
}

bool Shader::LoadShaderFile(std::string from, std::string &into)
{
	std::ifstream file;
	std::string temp;
	
	std::cout << " Loading shader text from " << from << std::endl << std::endl;
	
	file.open(from.c_str());
	if (!file.is_open()) {
		std::cout << " File does not exist !" << std::endl;
		return false;
		
	}
	while (!file.eof()) {
		getline(file, temp);
		into += temp + "\n";
		
	}
	
	file.close();
	std::cout << into << std::endl << std::endl;
	std::cout << " Loaded shader text !" << std::endl << std::endl;
	return true;

}

void Shader::SetDefaultAttributes()
{
	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, NORMAL_BUFFER, "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
}

bool Shader::LinkProgram()
{
	if (loadFailed)
	{
		return false;
	}

	glLinkProgram(program);

	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}