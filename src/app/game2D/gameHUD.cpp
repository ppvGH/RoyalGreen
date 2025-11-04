#include "gameHUD.h"
#include "sprite_renderer.h"

GameHUD::GameHUD(const std::string catLifeTexName):
	m_spriteCatLife(catLifeTexName)
{
}

void GameHUD::render(const SpriteRenderer& spriteRenderer, Shader& shader, glm::vec2 position, glm::vec2 size) const
{
	m_spriteCatLife.render(spriteRenderer, shader, position, size);
}