#include "framebuffer.h"
#include "../core/validation.h"
#include "../app/scene3D/scene_data.h"
#include <glad/glad.h>
#include <iostream>

Framebuffer::Framebuffer(int width, int height, char type):
	m_ID(0),
	m_width(width),
	m_height(height),
	m_tex()
{
	validation::require(width>0 && height>0,"Framebuffer dimensions must be positive");
    try {
	/* m_tex member is initialized in either one of the following branches */
	if (type == sceneData::FBOtypeColor) genColorFBO(width, height);
	else if (type == sceneData::FBOtypeDepth) genDepthFBO(width, height);
	else if (type == sceneData::FBOtypeCubeDepth) genCubeDepthFBO();
    else throw std::invalid_argument("Unknown framebuffer type");
    } catch(...) { if(m_ID)glDeleteFramebuffers(1,&m_ID);m_ID=0;throw; }
}


void Framebuffer::genColorFBO(int width, int height)
{
	/* Initializing m_tex as a color map.*/
	m_tex.genColorMap(TexParams(width, height), nullptr);

	/* Generating Color FBO. */
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex.getID(), 0);

	/* Check for FBO completeness. */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Framebuffer is incomplete");

	/* Unbind. */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::genDepthFBO(int width, int height)
{
	/* Initializing m_tex as a depth map.*/
	m_tex.genDepthMap(width, height);

	/* Generating Depth FBO. */
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_tex.getID(), 0);

	/* Disable color operation on this buffer. */
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	/* Check for completeness. */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Framebuffer is incomplete");

	/* Unbind. */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::genCubeDepthFBO()
{
	/* Initializing m_tex as a depth map.*/
	m_tex.genCubeDepthMap();

	/* Generating Depth FBO. (NOT 2D) */
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_tex.getID(), 0);

	/* Disable color operation on this buffer. */
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	/* Check for completeness. */
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Framebuffer is incomplete");

	/* Unbind. */
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


Framebuffer::~Framebuffer() { if(m_ID)glDeleteFramebuffers(1,&m_ID); }
