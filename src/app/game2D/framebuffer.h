#pragma once

#include "../../graphics/texture.h"
//####### DA SPOSTARE IN GRAPHICS INSIEME A SPRITERENDERER
class Framebuffer
{
public:

	Framebuffer() = delete;
	Framebuffer(int width, int height);

	void bind() const;
	void unbind() const;

	unsigned int getID() const { return m_ID; }
	const Texture& getTex() const { return m_tex; }
	const int getWidth() const { return m_width; }
	const int getHeight() const { return m_height; }

private:
	/* Identifier of the FB object. */
	unsigned int m_ID;
	/* Sizes of the Texture object. */
	int m_width, m_height;

	/* The FBO content gets drawn into a Texture object. */
	Texture m_tex;
};