#pragma once
#include <unordered_map>
#include <string>

#include "game_obj.h"
#include "sprite.h"

class Projectile
{
public:
	Projectile(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity, bool facingRight);


	void update(float dt);

	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

	/* Getters. */
	glm::vec2 getPosition() const { return m_body.getPosition(); }
	glm::vec2 getSize() const { return m_body.getSize(); }
	glm::vec2 getVelocity() const { return m_body.getVelocity(); }



private:
	GameObj m_body;		// logic
	Sprite m_sprite;	// render

	bool m_facingRight;
	
};
