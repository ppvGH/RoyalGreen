#pragma once

#include "game_obj.h"
#include "sprite.h"

class Background
{
public:
	/* Background sprite is not a sprite sheet so there is no need to include sheetRows and sheetCols as arguments. */
	Background(const std::string texName, glm::vec2 position, glm::vec2 size);

	
	void render(const SpriteRenderer& spriteRenderer, Shader& shader,
		glm::vec2 position = glm::vec2{0.0f}, glm::vec2 size = glm::vec2{0.0f}) const;


private:

	/* Only included for its position and size, not for physics. */
	GameObj m_body;

	Sprite m_sprite;

};