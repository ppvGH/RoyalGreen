#include "player.h"
#include "sprite_renderer.h"
#include "game_data.h"
#include "projectile_manager.h"

#include "../../core/utilities.h"
#include <iostream>

Player::Player(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity):
	m_body(position, size, velocity),
	m_sprite(texName, gameData::playerSheetRows, gameData::playerSheetColumns),
	m_state(State::Idle),
	m_prevState(State::Walk),		// different from m_state to start sprite animations
	m_facingRight(true),
	m_onGround(true),
	m_canMove(true),
	m_shotFired(false)
{
}

void Player::inputHandler(const ActionMap& actionMap2D, float dt, ProjectileManager& projectilesSys)
{
	if (!m_canMove) return;

	if (isIdle(actionMap2D)) atIdle();
	if (actionMap2D.ongoing(Action::P1MoveRight)) walk(true);
	if (actionMap2D.ongoing(Action::P1MoveLeft)) walk(false);

	if ((actionMap2D.justStarted(Action::P1Jump))) setJump();

	if (actionMap2D.justStarted(Action::P1Attack)) setAttack();

	//printNewState(actionMap2D);
}

bool Player::isIdle(const ActionMap& actionMap2D)
{
	/* P1 is Idle if its state is Idle or if there aren't any ongoing actions (walk, jump, attack). */

	/* Actions condition. */
	bool actions = !(actionMap2D.ongoing(Action::P1MoveRight) || actionMap2D.ongoing(Action::P1MoveLeft)); // walking
	/* States condition. */
	bool states = !(m_state == State::Jump || m_state == State::Attack);

	return (actions && states) || m_state == State::Idle;
}

void Player::atIdle()
{
	m_state = State::Idle;
	m_body.setVelocity(glm::vec2(0.0f));
	m_canMove = true;
	m_onGround = true;		// TODO check mechanism if build platforms
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

	/* Can move while jumping (horizontal velocity is set even when canWalk is false). */
	if(m_state != State::Jump) m_state = State::Walk;
}

void Player::setJump()
{
	/* First time entering the function: setting vertical speed to maxJumpSpeed and state to Jump. */
	if (m_state != State::Jump && m_onGround)
	{
		m_body.setVerticalVelocity(gameData::playerMaxJumpSpeed);
		m_onGround = false;
		m_state = State::Jump;
	}
}

void Player::jumpHandler(float dt)
{
	if (m_state != State::Jump) return;
	
	/* Update vertical speed. */
	m_body.setVerticalVelocity(m_body.getVerticalVelocity() - gameData::gravityAcceleration * dt);

	/* End of jump. */
	if (m_body.getVerticalPosition() < gameData::groundLevel) 
	{
		//printVec3(glm::vec3(m_body.getPosition(), 0.0f));
		m_body.setVerticalPosition(gameData::groundLevel);
		m_body.setVerticalVelocity(0.0f);
		m_state = State::Idle;
		m_onGround = true;
	}

}

void Player::setAttack()
{
	/* First time entering the function. P1 has to be moveable and on ground. */
	if (m_state != State::Attack && m_canMove && m_onGround)
	{
		m_state = State::Attack;
		m_canMove = false;
		m_body.setHorizontalVelocity(0.0f);	// stops the character
	}
}

void Player::attackHandler(ProjectileManager& projectilesSys)
{
	// enter function only if State::Attack is ongoing since 2+ frames
	if (m_state != State::Attack) return;	

	bool shootingFrameCond = m_sprite.getAnimator().getFrame() == gameData::playerShootFrame;
	
	if (shootingFrameCond && !m_shotFired)
	{
		shootArrow(projectilesSys);
		m_shotFired = true;		// shoots only once
	}
	else if (m_sprite.getAnimator().isAnimationFinished())
	{
		m_state = State::Idle;
		m_canMove = true;
		m_shotFired = false;	// can shoot again
	}

}

void Player::shootArrow(ProjectileManager& projectilesSys)
{	
	/* Projectile position adjusted for crossbow height. */
	glm::vec2 pos = m_body.getPosition() + glm::vec2(0.0f, 0.1f * m_body.getSize().y);
	if (m_facingRight) pos.x += 0.55f * m_body.getSize().x;
	else pos.x -= 0.05f * m_body.getSize().x;

	/* Projectile velocity. */
	glm::vec2 velocity = gameData::arrowSpeed * glm::vec2(1.0f, 0.0f);	// speed * direction
	if(!m_facingRight) velocity *= -1;		// invert speed if facing left

	/* Creates a projectile with given position, speed, size, verse (+-facingRight). */
	projectilesSys.emit(gameData::arrowTexName, pos, gameData::arrowSize, velocity, m_facingRight);
}

void Player::printNewState(const ActionMap& actionMap2D)
{
	static int count = -1;
	if (m_prevState != m_state) 
	{
		std::cout << "state: " << stateToString(m_state) << 1+count++ << std::endl;
		if(count!=0) std::cout << "prevState: " << stateToString(m_prevState) << count-1 << std::endl;
		std::cout << "canMove: " << (bool)m_canMove << std::endl;
		std::cout << "onGround: " << (bool)m_onGround << std::endl;
		std::cout << "isIdle: " << (bool)isIdle(actionMap2D) << std::endl;
		std::cout << " ----------------------------- " << std::endl;

	}
}

void Player::update(float dt, ProjectileManager& projectileSys)
{
	/* Texture update.*/
	updateAnimation();
	m_sprite.updateTexture(dt,m_facingRight);

	/* Position update. */
	m_body.updatePosition(dt);

	/* States behaviour update. */
	jumpHandler(dt);
	attackHandler(projectileSys);
}

void Player::render(const SpriteRenderer& spriteRenderer, Shader& shader) const
{
	m_sprite.render(spriteRenderer, shader, m_body.getPosition(), m_body.getSize());
}

void Player::resetPlayer()
{
	/* Reset position, velocity, state, facingRight. */
	m_body.setPosition(gameData::playerPosition);
	m_body.setVelocity(glm::vec2(0.0f));
	m_state = State::Idle;
	m_prevState = State::Walk;		
	m_facingRight = true;
	m_onGround = true;
	m_canMove = true;
	m_shotFired = false;
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