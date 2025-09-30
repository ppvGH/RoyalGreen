#pragma once

// Substitute includes with fwd declarations (ext classes used just by references)
//#include "../../graphics/shader.h"
//#include "../../graphics/texture.h" // da togliere quando creo classe sprite TODO
#include <glm/glm.hpp>

class Shader;
class Texture;
// FILE DA SPOSTARE IN GRAPHICS INSIEME A FBO ###############################################################
class SpriteRenderer
{
public:

	SpriteRenderer() = delete;
	SpriteRenderer(int width, int height);

	/* TODO: ADD ROTATION */
	void drawSprite(Shader& shader, Texture& texture, 
		glm::vec2 position, 
		glm::vec2 size, 
		glm::vec4 uvCoords = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)) const;


private:
	/* One quadVAO is used to render all sprites, just changing sizes and textures. */
	unsigned int m_quadVAO;

	/* Shader currently used to draw. PASSED BY ARGUMENT FROM GAME, so
	 * stuff know only who is the spriterenderer and what shader are they using. */
	//Shader& m_shader;

	/* Window sizes, needed for ortho projection matrix. */
	int m_width, m_height;

	/* VAO setup. */
	void initData();
};