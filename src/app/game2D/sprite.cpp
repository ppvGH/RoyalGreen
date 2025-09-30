#include "sprite.h"
#include "sprite_renderer.h"

//#include "../../core/utilities.h"
//#include <iostream>


Sprite::Sprite(const std::string texName, int sheetRows, int sheetColumns) :
	m_tex(ResourceManager::getTexture(texName)),
	m_animator(sheetRows, sheetColumns),
	m_uvCoords(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
{
}


void Sprite::setAnimation(int start, int end, int line, float fps)
{
	m_animator.setAnimation(start, end, line, fps);
}


void Sprite::updateTexture(float dt, bool facingRight)
{
	m_uvCoords = m_animator.updateUV(dt, facingRight);
}


void Sprite::render(const SpriteRenderer& spriteRenderer, Shader& shader, glm::vec2 position, glm::vec2 size) const
{
	spriteRenderer.drawSprite(shader, m_tex, position, size, m_uvCoords);
}


