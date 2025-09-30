#include "game_obj.h"
#include "game_data.h"
//#include <iostream>

GameObj::GameObj(glm::vec2 position, glm::vec2 size, glm::vec2 velocity) :
	m_position(position),
	m_size(size),
	m_velocity(velocity)
{
}

void GameObj::updatePosition(float dt)
{
	m_position += m_velocity * dt;
	/* For certain elements like the player it doesnt seem to be the real 0 (the left border of the screen),
	 * but that is just because sprite transparency occupies a consistent portion of the quad. */
	//if (m_position.x < 0.0f) m_position.x = 0.0f;
	//if (m_position.y < gameData::groundLevel) m_position.y = gameData::groundLevel;
}
