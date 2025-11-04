#pragma once

#include "player.h"
#include "cat.h"
#include "projectile_manager.h"

#include "../../core/collision.h"


/* Only collisions needed to be detected and managed are:
 > p1_proj  vs cat_gameObj
 > cat_proj vs p1_gameObj 
 */

class CollisionSystem
{
public:
	CollisionSystem(Player& player, Cat& cat, ProjectileManager& projectileSys);

	void checkCollision();

private:
	// References to game objects
	Player& m_player;
	Cat& m_cat;
	
	// pointers to
	ProjectileManager& m_projectileSystem;

	BoundingBox gameObjToBoundingBox(const GameObj& gameObj);

};