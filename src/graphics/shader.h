#pragma once
#include <glm/glm.hpp>
#include <string>


class Shader
{
public:
	/* Default constructor: initalizes ID to 0 */
	Shader() : m_ID(0) {};

	/* Creates, compiles and links shader program from vertex and fragment in a string format 
	 - arguments are given one by one to the compile private member function 
	 > returns *this to load shader programs in resource manager avoiding copies */
	Shader& linkProgram(const std::string& srcVert, const std::string& srcFrag);

	/* Activates shader
	 > returns *this */
	Shader& use();

	/* Getter for the shader program's ID */
	unsigned int getID() const;

	/* Uniforms */
	void setInt(const std::string& name, const int& value);
	void setFloat(const std::string& name, const float& value);
	void setVector3f(const std::string& name, const glm::vec3& vec);
	void setVector4f(const std::string& name, const glm::vec4 vec);
	void setMatrix4fv(const std::string& name, int count, bool transpose, const glm::mat4& matrix) const;

	
	/* Destructor */
	~Shader();


private:
	/* Shader program identifier. */
	unsigned int m_ID;

	/* Creates and compiles a shader from a string
	 > returns the shader id;
	 - type can only be "GL_VERTEX_SHADER" or "GL_FRAGMENT_SHADER"; */
	unsigned int compile(unsigned int type, const std::string& source);

	/* Shader compile error check function.
	 - id is the shader identifier;
	 - type can only be "GL_VERTEX_SHADER" or "GL_FRAGMENT_SHADER"; */
	void compileErrorCheck(unsigned int id, unsigned int type) const;

	/* Shader program link error check function.*/
	void linkErrorCheck() const;
};