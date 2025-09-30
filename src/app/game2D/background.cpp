#include "background.h"
#include "sprite_renderer.h"

Background::Background(const std::string texName, glm::vec2 position, glm::vec2 size) :
	m_body(position, size, glm::vec2(0.0f)),
	m_sprite(texName)
{
}


void Background::render(const SpriteRenderer& spriteRenderer, Shader& shader, glm::vec2 position, glm::vec2 size) const
{
	/* Default conditions. */
	if (position == glm::vec2{ 0.0f, 0.0f }) position = m_body.getPosition();
	if (size == glm::vec2{ 0.0f, 0.0f }) size = m_body.getSize();


	m_sprite.render(spriteRenderer, shader, position, size);
}