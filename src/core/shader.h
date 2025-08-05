#pragma once
#include <string>

class Shader
{
public:
	/* Default constructor: initalizes ID to 0 */
	Shader() : ID(0) {};

	/* Creates and compiles a shader from a string
	 > returns the shader id;
	 - type can only be "GL_VERTEX_SHADER" or "GL_FRAGMENT_SHADER"; */
	unsigned int compile(unsigned int type, const std::string& source);

	/* Creates, compiles and links shader program from vertex and fragment in a string format */
	void linkProgram(const std::string& srcVert, const std::string& srcFrag);

	/* Activates shader
	 > returns *this */
	Shader& use();

	/* Uniforms */
	// TODO


private:
	// shader program identifier	
	unsigned int ID;

	/* Shader compile error check function.
	 - id is the shader identifier;
	 - type can only be "GL_VERTEX_SHADER" or "GL_FRAGMENT_SHADER"; "*/
	void compileErrorCheck(unsigned int id, unsigned int type) const;

	/* Shader program link error check function.*/
	void linkErrorCheck() const ;
};