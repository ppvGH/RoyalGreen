#include "collision_system.h"
#include <iostream>


CollisionSystem::CollisionSystem(Player& player, Cat& cat, ProjectileManager& projectileSys):
	m_player(player),
	m_cat(cat),
	m_projectileSystem(projectileSys)
{
}

void CollisionSystem::checkCollision()
{
	
	for (auto& p : m_projectileSystem.getProjectiles())
	{
		if (p.get()->getOwner() == ProjectileOwner::Cat)	// cat's energy balls
		{
			if (intersectBB(gameObjToBoundingBox(p.get()->getGameObj()), m_player.getBB()))		// target: player
			{
				p.get()->setStatus(false);
				m_player.hasBeenHit();
			}
		}
		else if (p.get()->getOwner() == ProjectileOwner::Player)	// player's arrows
		{
			if (intersectBB(gameObjToBoundingBox(p.get()->getGameObj()), m_cat.getBB()))		// target: cat
			{
				p.get()->setStatus(false);
				m_cat.hasBeenHit();
			}
		}
	}
}

BoundingBox CollisionSystem::gameObjToBoundingBox(const GameObj& gameObj)
{
	return BoundingBox(gameObj.getPosition(), gameObj.getSize());
}
