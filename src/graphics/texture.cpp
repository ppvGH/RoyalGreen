#include "texture.h"
#include <iostream>


TexParams::TexParams(int width, int height, unsigned int format, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter):
	m_width(width),
	m_height(height),
	m_format(format),
	m_wrapS(wrapS),
	m_wrapT(wrapT),
	m_minFilter(minFilter),
	m_magFilter(magFilter)
{
}



Texture::Texture(const TexParams& params, unsigned char* data):
	m_ID(0)
{
	/* Generation and binding. */
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	/* Parameters setting.*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.m_wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.m_magFilter);

	/* Load data to the GPU. */
	glTexImage2D(GL_TEXTURE_2D, 0, params.m_format, params.m_width, params.m_height, 0, params.m_format, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D); not needed for now.

	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

