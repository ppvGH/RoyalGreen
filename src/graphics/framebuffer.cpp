#include "framebuffer.h"
#include <glad/glad.h>
#include <iostream>

Framebuffer::Framebuffer(int width, int height) :
	m_ID(0),
	m_width(width),
	m_height(height),
	m_tex(Texture(TexParams(width, height), nullptr) )
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex.getID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::FRAMEBUFFER:: FBO not complete." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}


void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

