#pragma once

// Substitute includes with fwd declarations (ext classes used just by references)
//#include "../../graphics/shader.h"
//#include "../../graphics/texture.h" // da togliere quando creo classe sprite TODO
#include <glm/glm.hpp>

class Shader;
class Texture;

class SpriteRenderer
{
public:

	SpriteRenderer();

	void drawSprite(Shader& shader, int screenWidth, int screenHeight, glm::vec3 position, glm::vec3 size, Texture& texture);





private:
	/* One quadVAO is used to render all sprites, just changing sizes and textures. */
	unsigned int m_quadVAO;

	/* Window sizes, needed for ortho projection matrix. */
	int m_width, m_height;

	/* VAO setup. */
	void initData();
};