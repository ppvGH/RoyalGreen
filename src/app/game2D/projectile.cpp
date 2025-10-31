#include "projectile.h"

Projectile::Projectile(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity, bool facingRight):
	m_sprite(texName),
	m_body(position, size, velocity),
	m_facingRight(facingRight)
{
	m_sprite.updateTexture(0, m_facingRight);
}

void Projectile::update(float dt)
{
	m_body.updatePosition(dt);
}

void Projectile::render(const SpriteRenderer& spriteRenderer, Shader& shader) const
{
	m_sprite.render(spriteRenderer, shader, m_body.getPosition(), m_body.getSize());
}


