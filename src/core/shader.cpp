#include "shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>		// for glm::value_ptr()
#include <iostream>



Shader& Shader::linkProgram(const std::string& srcVert, const std::string& srcFrag)
{
	// compile vertex and fragment shaders
	unsigned int vs = compile(GL_VERTEX_SHADER, srcVert);
	unsigned int fs = compile(GL_FRAGMENT_SHADER, srcFrag);

	// create and link shader program
	this->ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);
	glLinkProgram(ID);

	// link error check
	linkErrorCheck();
	
	// delete shaders after linking
	glDeleteShader(vs);
	glDeleteShader(fs);

	return *this;
}

Shader& Shader::use()
{
	glUseProgram(ID);
	// returns *this for chaining [e.g. shader.use().setInt(...)]
	return *this;
}

unsigned int Shader::getID() const
{
	return ID;
}

void Shader::setUniformMatrix4fv(const std::string& name, int count, bool transpose, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, transpose, glm::value_ptr(matrix));
}

Shader::~Shader()
{
	// if exists it deletes it
	if (ID != 0) 
	{
		glDeleteProgram(ID);
	}
}

unsigned int Shader::compile(unsigned int type, const std::string& source)
{
	// shader identifier
	unsigned int id;

	// std::string to C-string conversion
	const char* src = source.c_str();

	// create and compile shader
	id = glCreateShader(type);
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	// compile error check
	compileErrorCheck(id, type);

	return id;

}

void Shader::compileErrorCheck(unsigned int id, unsigned int type) const
{
	int success;
	char infoLog[1024];

	// checking for shader type
	std::string errMsg = "";
	if (type == GL_VERTEX_SHADER) errMsg = "VERTEX SHADER FAILURE";
	else if (type == GL_FRAGMENT_SHADER) errMsg = "FRAGMENT SHADER FAILURE";

	// check for error: if success is false print error message
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER: Compile-time error: " << errMsg << "\n" << infoLog
			<< "\n -- --------------------------------- --" << std::endl;
	}
}

void Shader::linkErrorCheck() const
{
	int success;
	char infoLog[1024];

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER: Link-time error: SHADER PROGRAM FAILURE\n" << infoLog
			<< "\n -- --------------------------------- --" << std::endl;
	}
}