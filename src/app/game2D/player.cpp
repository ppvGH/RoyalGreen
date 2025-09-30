#include "player.h"
#include "sprite_renderer.h"
#include "game_data.h"

#include "../../core/utilities.h"
#include <iostream>

Player::Player(const std::string texName, glm::vec2 position, glm::vec2 size, float walkSpeed):
	m_body(position, size, glm::vec2(0.0f)),
	m_sprite(texName, gameData::playerSheetRows, gameData::playerSheetColumns),
	m_state(State::Idle),
	m_prevState(State::Walk),		// different from m_state to start sprite animations
	m_facingRight(true),
	m_canWalk(true)
{
}

void Player::inputHandler(const ActionMap& actionMap2D, float dt)
{
	if (isIdle(actionMap2D)) atIdle();
	if (actionMap2D.ongoing(Action::P1MoveRight)) walk(true);
	if (actionMap2D.ongoing(Action::P1MoveLeft)) walk(false);
	if (actionMap2D.justStarted(Action::P1Jump) || m_state == State::Jump) jump(dt);
}

bool Player::isIdle(const ActionMap& actionMap2D)
{
	return !actionMap2D.ongoing(Action::P1MoveRight) &&
		   !actionMap2D.ongoing(Action::P1MoveLeft) &&
		   !(actionMap2D.ongoing(Action::P1Jump) || m_state == State::Jump);
}

void Player::atIdle()
{
	m_body.setVelocity(glm::vec2(0.0f));
	m_state = State::Idle;
}

void Player::walk(bool facingRight)
{
	/* Speed depends on orientation. */
	if (facingRight) m_body.setHorizontalVelocity(gameData::playerWalkSpeed);
	else m_body.setHorizontalVelocity(-gameData::playerWalkSpeed);
	m_facingRight = facingRight;	// saving orientation for sprite rendering

	/* It doesnt seem to be the real 0 (the left border of the screen),
	 * but that is just because sprite transparency occupies a consistent portion of the quad. */
	if (m_body.getHorizontalPosition() < 0.0f) m_body.setHorizontalPosition(0.0f);

	/* Can move while jumping. */
	if(m_state != State::Jump) m_state = State::Walk;
}

void Player::jump(float dt)
{
	/* First time entering the function: setting vertical speed to maxJumpSpeed and state to Jump. */
	if (m_state != State::Jump && m_canWalk)
	{
		m_body.setVerticalVelocity(gameData::playerMaxJumpSpeed);
		m_canWalk = false;
		m_state = State::Jump;
	}
	/* Update vertical speed. */
	if(m_state == State::Jump) m_body.setVerticalVelocity(m_body.getVerticalVelocity() - gameData::gravityAcceleration * dt);

	/* End of jump. */
	if (m_body.getVerticalPosition() < gameData::groundLevel) 
	{
		//printVec3(glm::vec3(m_body.getPosition(), 0.0f));
		m_body.setVerticalPosition(gameData::groundLevel);
		m_body.setVerticalVelocity(0.0f);
		m_state = State::Idle;
		m_canWalk = true;
	}

}

void Player::update(float dt)
{
	updateAnimation();

	m_body.updatePosition(dt);
	m_sprite.updateTexture(dt,m_facingRight);
}


void Player::render(const SpriteRenderer& spriteRenderer, Shader& shader) const
{
	m_sprite.render(spriteRenderer, shader, m_body.getPosition(), m_body.getSize());
}

void Player::setAnimation(int start, int end, int line, float fps)
{
	m_sprite.setAnimation(start, end, line, fps);
}

void Player::updateAnimation()
{
	/* Only set a new animation when the state changes. */
	if (m_state != m_prevState)
	{
		/* operator[] for unordered_map gives error because playerAnimations is declared const.
		 * So the uses of the function at() solves this issue. */
		const AnimationData& anim = gameData::playerAnimations.at(m_state);
		m_sprite.setAnimation(anim.startFrame, anim.endFrame, anim.animationLine, anim.fps);
		m_prevState = m_state;
	}
}