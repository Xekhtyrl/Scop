#include "Shader.hpp"
#include "CreateShader.hpp"

// Shader::Shader(const char* vertexCode, const char* fragmentCode)
// {
// 	unsigned int vertex, fragment;
	
// 	CompileShader(vertex, vertexCode, GL_VERTEX_SHADER);
// 	CompileShader(fragment, fragmentCode, GL_FRAGMENT_SHADER);
// 	CreateShaderProgram(vertex, fragment);
// 	glDeleteShader(vertex);
// 	glDeleteShader(fragment);
// }

Shader::Shader(std::string vertexFilePath, std::string fragmentFilePath) {

	std::cout << "Shader Constructor called" << std::endl;
	std::string vShaderCode, fShaderCode;
	unsigned int vertex, fragment;
	
	try {
		vShaderCode = fileToStr(vertexFilePath);
		fShaderCode = fileToStr(fragmentFilePath);
	} catch (...) {
		std::cerr << "Error: Shader constructor failed.\n";
		throw;
	}
	
	CompileShader(vertex, vShaderCode.c_str(), GL_VERTEX_SHADER);
	CompileShader(fragment, fShaderCode.c_str(), GL_FRAGMENT_SHADER);
	CreateShaderProgram(vertex, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {

	std::cout << "Shader Destroyer called" << std::endl;

    if (ID != 0)
        glDeleteProgram(ID);

}

void Shader::CompileShader(unsigned int& shader, const char* shaderCode, unsigned int type) {
	int success;
	char infoLog[512];

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
}

void Shader::CreateShaderProgram(unsigned int vertex, unsigned int fragment) {
	int success;
	char infoLog[512];
	
	// CompileShader(frag, CreateShader().getContent().c_str(), GL_FRAGMENT_SHADER);
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	// glAttachShader(ID, frag);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// glDeleteShader(frag);
}

void Shader::use() 
{ 
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

// void Shader::setFloats(const std::string &name, float r, float g, float b, float a) const
// { 
// 	glUniform4f(glGetUniformLocation(ID, name.c_str()), r, g, b, a); 
// }

void Shader::setMat(const char *name, const float* array) {
	int uniformLocation = glGetUniformLocation(ID, name);
	glUniformMatrix4fv(uniformLocation, 1, GL_TRUE, array);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, const vec3 &value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setVec4(const std::string &name, const vec4 &value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2], value[3]);
}


void Shader::addShader(const char *shaderCode, unsigned int type) {
	unsigned int shader;
	int success;
	char infoLog[512];

	CompileShader(shader, shaderCode, type);
	glAttachShader(ID, shader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(shader);
}

unsigned int Shader::getID() {
	return ID;
}