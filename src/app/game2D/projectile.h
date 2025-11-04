#pragma once
#include <unordered_map>
#include <string>

#include "game_obj.h"
#include "sprite.h"

enum class ProjectileOwner
{
	Player,
	Cat,	
};

class Projectile
{
public:
	Projectile(const std::string& texName, 
		glm::vec2 position, glm::vec2 size, glm::vec2 velocity, bool facingRight, 
		ProjectileOwner owner);


	void update(float dt);

	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

	/* Getters. */
	const GameObj& getGameObj() const { return m_body; }
	glm::vec2 getPosition() const { return m_body.getPosition(); }
	glm::vec2 getSize() const { return m_body.getSize(); }
	glm::vec2 getVelocity() const { return m_body.getVelocity(); }
	ProjectileOwner getOwner() const { return m_owner; }
	bool getStatus() const { return m_active; }

	/* Setters. */
	void setStatus(bool active) { m_active = active; }	// false if collision



private:
	GameObj m_body;		// logic
	Sprite m_sprite;	// render

	ProjectileOwner m_owner;		// who shot the projectile

	bool m_facingRight;	// verse of projectile
	bool m_active = true;	// false if collided

	
};
