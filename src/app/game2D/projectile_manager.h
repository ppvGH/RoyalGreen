#pragma once

#include <memory>	// for smart pointers, needed to manage lifespan
#include <vector>
#include "projectile.h"

class ProjectileManager
{
public:
	ProjectileManager() = default;

	/* Spawn projectile. */
	void emit(const std::string& texName, glm::vec2 pos, glm::vec2 size, glm::vec2 vel, bool facingRight);

	/* Update projectile system. */
	void update(float dt, float rightLim, float leftLim, float topLim, float botLim);

	/* Checks if there is any projectiles in the projectilesContainer and if yes, clears them. */
	void resetProjSys();

	/* Draw every projectiles. */
	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

private:

	std::vector<std::unique_ptr<Projectile>> m_projectilesContainer;

	/* Delete projectile. */
	void cancel(float rightLim, float leftLim, float topLim, float botLim);
};