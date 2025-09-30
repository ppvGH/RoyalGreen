#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../../core/resource_manager.h"
#include "animator.h"
class SpriteRenderer;


// #########################################################################
// ################################ Sprite #################################
// #########################################################################

/* Manages spritesheets and textures. */
class Sprite
{
public:

	/* Sheet rows and cols are only for sprite sheets. */
	Sprite(const std::string texName, int sheetRows = 1, int sheetColumns = 1);


	void setAnimation(int start, int end, int line, float fps);
	void updateTexture(float dt, bool facingRight);	// name clearer than just update
	void render(const SpriteRenderer& spriteRenderer, Shader& shader, glm::vec2 position, glm::vec2 size) const;

	const Texture& getTex() const { return m_tex; }

private:
	Texture& m_tex;

	Animator m_animator;
	glm::vec4 m_uvCoords;

};

