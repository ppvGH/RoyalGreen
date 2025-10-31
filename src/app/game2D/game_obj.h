#pragma once

#include <glm/glm.hpp>

class GameObj
{
public:
	
	/* Main constructor. */
	GameObj(glm::vec2 position, glm::vec2 size, glm::vec2 velocity);

	/* Position methods.*/
	glm::vec2 getPosition() const { return m_position; }
	float getHorizontalPosition() const { return m_position.x; }
	float getVerticalPosition() const { return m_position.y; }

	void setPosition(glm::vec2 position) { m_position = position; }
	void setHorizontalPosition(float horizontalPosition) { m_position.x = horizontalPosition; }
	void setVerticalPosition(float verticalPosition) { m_position.y = verticalPosition; }

	/* Size methods. */
	glm::vec2 getSize() const { return m_size; }
	void setSize(glm::vec2 size) { m_size = size; }

	/* Velocity methods. */
	glm::vec2 getVelocity() const { return m_velocity; }
	float getHorizontalVelocity() const { return m_velocity.x; }
	float getVerticalVelocity() const { return m_velocity.y; }

	void setVelocity(glm::vec2 velocity) { m_velocity = velocity; }
	void setHorizontalVelocity(float horizontalSpeed) { m_velocity.x = horizontalSpeed; }
	void setVerticalVelocity(float verticalSpeed) { m_velocity.y = verticalSpeed; }

	/* Update gameObj logic. */
	void updatePosition(float dt);


private:

	glm::vec2 m_position;
	glm::vec2 m_size;
	glm::vec2 m_velocity;



};