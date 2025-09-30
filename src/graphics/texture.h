#pragma once

#include "../core/commonGL.h"

/* Parameters for textures. */


class TexParams
{
public:

	// not needed from input
	int m_width;
	// not needed from input
	int m_height;
	unsigned int m_format;
	unsigned int m_wrapS;			// GL_REPEAT 
	unsigned int m_wrapT;			// GL_REPEAT 
	unsigned int m_minFilter;		// GL_LINEAR
	unsigned int m_magFilter;		// GL_LINEAR

	TexParams();

	TexParams(int width, int height,
		unsigned int format = GL_RGBA,
		unsigned int wrapS = GL_REPEAT,
		unsigned int wrapT = GL_REPEAT,
		unsigned int minFilter = GL_LINEAR,
		unsigned int magFilter = GL_LINEAR);
};

class Texture
{
public:

	Texture() = default;
	Texture(const TexParams& params, unsigned char* data);

	void bind() const;
	void unbind() const;
	unsigned int getID() const { return m_ID; };

private:
	/* Texture identifier. */
	unsigned int m_ID;
	
};

