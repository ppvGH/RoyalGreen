#include "shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <stdexcept>
Shader& Shader::linkProgram(const std::string& v,const std::string& f) { return buildProgram({{GL_VERTEX_SHADER,v},{GL_FRAGMENT_SHADER,f}}); }
Shader& Shader::linkProgram(const std::string& v,const std::string& g,const std::string& f) { return buildProgram({{GL_VERTEX_SHADER,v},{GL_GEOMETRY_SHADER,g},{GL_FRAGMENT_SHADER,f}}); }
Shader& Shader::buildProgram(const std::vector<std::pair<unsigned int,std::string>>& sources) {
    std::vector<unsigned int> stages;
    unsigned int program=0;
    try {
        for(const auto& source:sources) stages.push_back(compile(source.first,source.second));
        program=glCreateProgram();
        if(!program)throw std::runtime_error("Cannot allocate shader program");
        for(auto stage:stages)glAttachShader(program,stage);
        glLinkProgram(program);
        int ok=0; glGetProgramiv(program,GL_LINK_STATUS,&ok);
        if(!ok) { char log[2048]={}; glGetProgramInfoLog(program,sizeof(log),nullptr,log); throw std::runtime_error(std::string("Shader link failed: ")+log); }
        for(auto stage:stages)glDeleteShader(stage);
        if(m_ID)glDeleteProgram(m_ID); m_ID=program;
    } catch(...) { for(auto stage:stages)glDeleteShader(stage); if(program)glDeleteProgram(program); throw; }
    return *this;
}
unsigned int Shader::compile(unsigned int type,const std::string& source) {
    if(source.empty())throw std::runtime_error("Shader source is empty");
    if(!glCreateShader)throw std::logic_error("OpenGL is not initialized");
    unsigned int id=glCreateShader(type);
    if(!id)throw std::runtime_error("Cannot allocate shader stage");
    const char* data=source.c_str();glShaderSource(id,1,&data,nullptr);glCompileShader(id);
    int ok=0;glGetShaderiv(id,GL_COMPILE_STATUS,&ok);
    if(!ok) { char log[2048]={};glGetShaderInfoLog(id,sizeof(log),nullptr,log);glDeleteShader(id);throw std::runtime_error(std::string("Shader compilation failed: ")+log); }
    return id;
}
Shader::~Shader() { if(m_ID)glDeleteProgram(m_ID); }
Shader::Shader(Shader&& other) noexcept:m_ID(std::exchange(other.m_ID,0)) {}
Shader& Shader::operator=(Shader&& other) noexcept {
    if(this!=&other) { if(m_ID)glDeleteProgram(m_ID);m_ID=std::exchange(other.m_ID,0); } return *this;
}
Shader& Shader::use()
{
	if(!m_ID)throw std::logic_error("Shader is not linked");
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
