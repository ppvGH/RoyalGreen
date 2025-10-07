#pragma once

#include "../core/commonGL.h"

/* Parameters for textures. */


class TexParams
{
public:

	// If not in FBO, set it to 0. It will be retrieved from the ResourceManager.
	int m_width;
	// If not in FBO, set it to 0. It will be retrieved from the ResourceManager.
	int m_height;
	unsigned int m_format;
	unsigned int m_wrapS;			// GL_REPEAT 
	unsigned int m_wrapT;			// GL_REPEAT 
	unsigned int m_minFilter;		// GL_LINEAR
	unsigned int m_magFilter;		// GL_LINEAR


	// If not in FBO, set width and height to 0. They will be retrieved from the ResourceManager.
	TexParams(int width = 0, int height = 0,
		unsigned int format = GL_RGBA,
		unsigned int wrapS = GL_REPEAT,
		unsigned int wrapT = GL_REPEAT,
		unsigned int minFilter = GL_LINEAR,
		unsigned int magFilter = GL_LINEAR);
};

class Texture
{
public:

	Texture() : m_ID(0) {};
	Texture(const TexParams& params, unsigned char* data);

	void bind() const;
	void unbind() const;
	unsigned int getID() const { return m_ID; };

private:
	/* Texture identifier. */
	unsigned int m_ID;
	
};

