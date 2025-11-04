#pragma once

#include <string>

#include "sprite.h"

class GameHUD
{
public:

	GameHUD(const std::string catLifeTexName);

	void render(const SpriteRenderer& spriteRenderer, Shader& shader, glm::vec2 position, glm::vec2 size) const;

private:
	Sprite m_spriteCatLife;
};