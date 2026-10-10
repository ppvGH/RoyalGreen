#include "texture.h"
#include "../core/validation.h"
#include <utility>
#include <stdexcept>
#include "../app/scene3D/scene_data.h"
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
	genColorMap(params, data);
}

void Texture::genColorMap(const TexParams& params, unsigned char* data)
{
    validation::require(params.m_width>0 && params.m_height>0,"Texture dimensions must be positive");

	/* Check if m_ID is already set to another value different than 0. */
	if (m_ID != 0)
	{
		throw std::logic_error("Texture is already initialized");
	}

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

	glBindTexture(m_target, 0);
}

void Texture::genDepthMap(int width, int height)
{
    validation::require(width>0 && height>0,"Depth texture dimensions must be positive");
	/* Check if m_ID is already set to another value different than 0. */
	if (m_ID != 0)
	{
		throw std::logic_error("Texture is already initialized");
	}

	/* Generation and binding. */
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	/* Parameters setting.*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// to avoid visible wrapping near light borders.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	/* Create depth texture. */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	/* Unbind texture. */
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::genCubeDepthMap()
{
	/* Check if m_ID is already set to another value different than 0. */
	if (m_ID != 0)
	{
		throw std::logic_error("Texture is already initialized");
	}

	/* Generation and binding for a cube map texture. */
	glGenTextures(1, &m_ID);
    m_target=GL_TEXTURE_CUBE_MAP;
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

	/* Parameters setting.*/
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/* Create cube depth texture. */
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			sceneData::shadowWidth, sceneData::shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	
	/* Unbind texture. */
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void Texture::bind() const
{
	glBindTexture(m_target, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(m_target, 0);
}


Texture::~Texture() { if(m_ID) glDeleteTextures(1,&m_ID); }
Texture::Texture(Texture&& other) noexcept:m_ID(std::exchange(other.m_ID,0)),m_target(other.m_target) {}
Texture& Texture::operator=(Texture&& other) noexcept {
    if(this!=&other) { if(m_ID)glDeleteTextures(1,&m_ID); m_ID=std::exchange(other.m_ID,0); m_target=other.m_target; }
    return *this;
}
