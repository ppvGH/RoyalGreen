#include "projectile_manager.h"
#include <iostream>

void ProjectileManager::emit(const std::string& texName, glm::vec2 pos, glm::vec2 size, glm::vec2 vel, bool facingRight)
{
	m_projectilesContainer.push_back(std::make_unique<Projectile>(texName, pos, size, vel, facingRight));
}

void ProjectileManager::cancel(float rightLim, float leftLim, float topLim, float botLim)
{	
	// Starting fron the end, because when erasing an element, the remaining get shifted back (indices issues)
	for (int i = m_projectilesContainer.size() - 1; i >= 0; i--)
	{
		glm::vec2 projPos = m_projectilesContainer[i]->getPosition();
		
		if (projPos.x > rightLim || projPos.x < leftLim) m_projectilesContainer.erase(m_projectilesContainer.begin() + i);	//proj out right/left
		else if(projPos.y > topLim || projPos.y < botLim) m_projectilesContainer.erase(m_projectilesContainer.begin() + i);	//proj out top/bot
	}
}

void ProjectileManager::update(float dt, float rightLim, float leftLim, float topLim, float botLim)
{
	for (auto& proj : m_projectilesContainer) proj->update(dt);	// update positions
	cancel(rightLim, leftLim, topLim, botLim);	// remove 
}

void ProjectileManager::resetProjSys()
{
	if (!m_projectilesContainer.empty()) m_projectilesContainer.clear();
}


void ProjectileManager::render(const SpriteRenderer& spriteRenderer, Shader& shader) const
{
	for (const auto& proj : m_projectilesContainer) proj->render(spriteRenderer, shader);
}