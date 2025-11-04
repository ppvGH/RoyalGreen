#pragma once

#include <memory>	// for smart pointers, needed to manage lifespan
#include <vector>
#include "projectile.h"

class ProjectileManager
{
public:
	/* Default constructors initializes the projectiles container. */
	ProjectileManager() = default;

	/* Spawn projectile. Arguments are same of projectile constructor. */
	void emit(const std::string& texName, 
		glm::vec2 pos, glm::vec2 size, glm::vec2 vel, bool facingRight,
		ProjectileOwner owner);

	/* Updates projectiles position and if they are out of camera view it removes them. */
	void update(float dt, float rightLim, float leftLim, float topLim, float botLim);

	/* Checks if there is any projectiles in the projectilesContainer and if yes, clears them. */
	void resetProjSys();

	/* Draw every projectiles. */
	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

	/* Getter for collision system. */
	std::vector<std::unique_ptr<Projectile>>& getProjectiles() { return m_projectilesContainer; }

private:

	std::vector<std::unique_ptr<Projectile>> m_projectilesContainer;

	/* Delete projectile. */
	void cancel(float rightLim, float leftLim, float topLim, float botLim);
};