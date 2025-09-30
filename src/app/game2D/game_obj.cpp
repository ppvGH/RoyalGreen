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
}
