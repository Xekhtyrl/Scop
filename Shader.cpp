#include "Shader.hpp"
#include "CreateShader.hpp"


/// @brief Shader Constructor that load and compile the shader files (fragment and Vertex) and send it to openGL
/// @param vertexFilePath Vertex shader file path
/// @param fragmentFilePath Fragment shader file path
/// @throw throw an exception when one is caught if the conversion from file to string failed
/// @throw throw an exception when Shader Compilation failed
/// @throw throw an exception when Shader Program Creation failed
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
	
	if (!CompileShader(vertex, vShaderCode.c_str(), GL_VERTEX_SHADER)){
		throw std::runtime_error("Vertex Shader compilation failed");
	}
	if (!CompileShader(fragment, fShaderCode.c_str(), GL_FRAGMENT_SHADER)){
		glDeleteShader(vertex);
		throw std::runtime_error("Fragment Shader compilation failed");
	}
	if (!CreateShaderProgram(vertex, fragment)){
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		throw std::runtime_error("Shader Program Creation failed");
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/// @brief delete the shader program
Shader::~Shader() {

	std::cout << "Shader Destroyer called" << std::endl;

    if (ID != 0)
        glDeleteProgram(ID);

}


/// @brief constructor subfunction that compile the shader code and set the shadr ID needed for the the shader program
/// @param shader reference of an empty variable that will be set for the creation of the shader program
/// @param shaderCode shader file code stocked into a string that will be compiled
/// @param type Vertex or Fragment openGL shader code (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
/// @return 0 on failure, and non null value on success
int Shader::CompileShader(unsigned int& shader, const char* shaderCode, unsigned int type) {
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
	}
	return success;
}

/// @brief constructor subfunction called to create the shader program with the two compiled shaders id (vertex and fragment) to attach to it, and set the program id to the Shader class attribute ID
/// @param vertex Compiled vertex id
/// @param fragment compiled fragment id
/// @return 0 on failure, and non null value on success
int Shader::CreateShaderProgram(unsigned int vertex, unsigned int fragment) {
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
	return success;
	// glDeleteShader(frag);
}

/// @brief call function to use the shader program
void Shader::use() 
{ 
    glUseProgram(ID);
}

//________________ Set of functions that set variables 'values' to a 'named' variable in the shader program_____________________//

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

// // attempt to add other shader post hoc to the program
// void Shader::addShader(const char *shaderCode, unsigned int type) {
// 	unsigned int shader;
// 	int success;
// 	char infoLog[512];

// 	CompileShader(shader, shaderCode, type);
// 	glAttachShader(ID, shader);
// 	glLinkProgram(ID);
// 	glGetProgramiv(ID, GL_LINK_STATUS, &success);
// 	if(!success)
// 	{
// 		glGetProgramInfoLog(ID, 512, NULL, infoLog);
// 		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
// 	}
// 	glDeleteShader(shader);
// }

unsigned int Shader::getID() {
	return ID;
}