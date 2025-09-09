#pragma once

#include "texture.h"

class Framebuffer
{
public:

	Framebuffer(int width, int height);

	void bind();
	void unbind();

	unsigned int getID() const { return m_ID; }
	const Texture& getTex() const { return m_tex; }
	const int* getDims() const { const int ret[2] = { m_width, m_height };  return ret; }

private:
	unsigned int m_ID;
	int m_width;
	int m_height;

	Texture m_tex;
};