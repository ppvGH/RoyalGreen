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
	this->m_ID = glCreateProgram();
	glAttachShader(m_ID, vs);
	glAttachShader(m_ID, fs);
	glLinkProgram(m_ID);

	// link error check
	linkErrorCheck();
	
	// delete shaders after linking
	glDeleteShader(vs);
	glDeleteShader(fs);

	return *this;
}

Shader& Shader::use()
{
	glUseProgram(m_ID);
	// returns *this for chaining [e.g. shader.use().setInt(...)]
	return *this;
}

unsigned int Shader::getID() const
{
	return m_ID;
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setVector2f(const std::string& name, const glm::vec2& vec) const
{
	glUniform2f(glGetUniformLocation(m_ID, name.c_str()), vec.x, vec.y);
}

void Shader::setVector3f(const std::string& name, const glm::vec3& vec) const
{
	glUniform3f(glGetUniformLocation(m_ID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setVector4f(const std::string& name, const glm::vec4& vec) const
{
	glUniform4f(glGetUniformLocation(m_ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setMatrix4fv(const std::string& name, int count, bool transpose, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), count, transpose, glm::value_ptr(matrix));
}


Shader::~Shader()
{
	// if exists it deletes it
	if (m_ID != 0) 
	{
		glDeleteProgram(m_ID);
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

	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ID, 1024, NULL, infoLog);
		std::cerr << "ERROR::SHADER: Link-time error: SHADER PROGRAM FAILURE\n" << infoLog
			<< "\n -- --------------------------------- --" << std::endl;
	}
}