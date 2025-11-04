#include "cat.h"

#include "game_data.h"
#include "projectile_manager.h"
#include <cmath>
#include <iostream>
#include "../../core/utilities.h"

Cat::Cat(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity) :
	m_body(position, size, velocity),
	m_sprite(texName, gameData::catSheetRows, gameData::catSheetColumns),
	m_BB(position + gameData::bbOffsetPosition, gameData::bbSize),
	m_state(State::Idle),
	m_prevState(State::Walk),
	m_lives(gameData::catLives),
	m_facingRight(false),
	m_canMove(true),
	m_onGround(true),
	m_attackTimer(0.0f),
	m_setJumpMove(false),
	m_energyBallFired(false),
	m_blastCharge(true),
	m_blastTimer(0.0f),
	m_blastCount(0)
{
}

void Cat::render(const SpriteRenderer& spriteRenderer, Shader& shader) const
{
	/* TODO: If here shader becomes a member (m_shader) which is update when a change of state happens.
	 * i.e. when hit the shader flashes, could be cool. */
	m_sprite.render(spriteRenderer, shader, m_body.getPosition(), m_body.getSize());
}

void Cat::autoInput(float dt, float playerXpos)
{
	incrementTimers(dt);
	if (!m_onGround || !m_canMove) return;	// if cat is on air dont modify its state
	else if (abs(m_body.getHorizontalPosition() - playerXpos) < 200.0f) setJumpMove();	// blast trigger by position
	else if (gameData::blastCharge.count(m_lives) && m_blastCharge) { setJumpMove(); m_blastCharge = false;	}	// blast trigger by lives
	else if (m_attackTimer > 2.0f) setAttack();

}

void Cat::incrementTimers(float dt)
{
	m_attackTimer += dt;
}

void Cat::setAttack()
{
	m_attackTimer = 0.0f;	// reset accumulator
	m_state = State::Attack;
}

void Cat::attackHandler(float dt, ProjectileManager& projectileSys, glm::vec2 playerPosition)
{
	if (m_state != State::Attack) return;		

	bool shootCond = m_sprite.getAnimator().getFrame() == gameData::catShootFrame;
	if (shootCond && !m_energyBallFired)		
	{
		shootEnergyBall(projectileSys, playerPosition, gameData::energyBallSpeed);	// cast energy ball on target = playerPosition
		m_energyBallFired = true;
	}
	else if (m_sprite.getAnimator().isAnimationFinished())
	{
		m_state = State::Idle;
		m_energyBallFired = false;
	}
}

void Cat::shootEnergyBall(ProjectileManager& projectilesSys, glm::vec2 target, float speed)
{
	/* Projectile starting position. */
	glm::vec2 pos = m_body.getPosition();
	if (m_onGround)		// valid only for balls shooted while on ground
	{
		if (m_facingRight) pos += glm::vec2(m_body.getSize().x, 0.6f * gameData::energyBallSize.y);
		else pos += glm::vec2(-gameData::energyBallSize.x, 0.6f * gameData::energyBallSize.y);
	}
	else pos.x += 0.5f * m_body.getSize().x;

	/* Projectile direction. */
	glm::vec2 dir = glm::normalize(target - m_body.getPosition());

	/* Projectile velocity. */
	glm::vec2 vel = speed * dir;

	/* Creates projectile. */
	projectilesSys.emit(gameData::energyBallTexName, pos, gameData::energyBallSize, vel, m_facingRight, ProjectileOwner::Cat);

}

void Cat::setJumpMove()
{
	m_body.setVerticalVelocity(gameData::catMaxJumpSpeed);
	m_onGround = false;
	m_setJumpMove = true;
	m_state = State::Jump;
}

void Cat::jumpMoveHandler(float dt, ProjectileManager& projectileSys, float playerXpos, float cameraPos)
{
	if (m_state != State::Jump) return;
	
	if (m_setJumpMove == true)	// first time entering function
	{
		if (m_body.getHorizontalPosition() < playerXpos) m_body.setHorizontalVelocity(gameData::catWalkSpeed);
		else m_body.setHorizontalVelocity(-gameData::catWalkSpeed);
		m_setJumpMove = false;
	}
	/* Update vertical speed. */
	m_body.setVerticalVelocity(m_body.getVerticalVelocity() - gameData::gravityAcceleration * dt);

	/* When at max height, stop, wait and cast spells. */
	// stop the jump mid air (overrides previous update)
	bool blastCondition = (m_body.getVerticalVelocity() < 0.0f) && (m_blastCount < 4);
	if (blastCondition)
	{
		m_body.setVelocity(glm::vec2(0.0f));			// overrides vert speed update
		m_blastTimer += dt;								// increments timer

		if (m_blastTimer > 0.5f)		// blast every 0.5 sec
		{
			/* Shoots 3 4 5 6 balls. */
			int ballCount = m_blastCount + 3;
			float frac = 600.0f / static_cast<float>(ballCount - 1);
			for (int i = 0; i < ballCount; i++)
			{
				glm::vec2 target = glm::vec2(cameraPos + frac * i, gameData::groundLevel);
				float speed = gameData::energyBallSpeed - gameData::blastSpeedAttenuation * ballCount;
				shootEnergyBall(projectileSys, target, speed);
			}
			m_blastCount++;
			m_blastTimer = 0.0f;
		}
	}
	else if (m_blastCount == 4) //reset horizontal speed when blast is over. inverse of initial setting
	{
		if (m_body.getHorizontalPosition() < playerXpos) m_body.setHorizontalVelocity(- gameData::catWalkSpeed);
		else m_body.setHorizontalVelocity(gameData::catWalkSpeed);
	}

	/* End condition of the jump. */
	if (m_body.getVerticalPosition() < gameData::groundLevel)
	{
		m_body.setVerticalPosition(gameData::groundLevel);				// snap body to ground
		m_body.setVelocity(glm::vec2(0.0f));							// stops  motion
		m_onGround = true;
		m_blastCount = 0;				// reset to enable blast for next jump
		m_attackTimer = 0.0f;			// cat can't shoot as soon as he lands
		m_state = State::Idle;
	}



}

void Cat::updateFacing(float playerXpos)
{
	if (m_body.getPosition().x < playerXpos) m_facingRight = true;
	else m_facingRight = false;
}

void Cat::hasBeenHit()
{
	if (m_lives > 0) m_lives--;
	m_blastCharge = true;
}

void Cat::deathHandler()
{
	if (m_lives == 0 && m_state != State::Hit)	// only first time in
	{
		m_canMove = false;	// blocks inputs
		m_state = State::Hit;
		return;		// skips the rest of the function
	}
	else if (m_state == State::Hit && m_sprite.getAnimator().isAnimationFinished())
	{
		m_state = State::Dead;
	}

}

void Cat::update(float dt, ProjectileManager& projectileSys, glm::vec2 playerPosition, float cameraPos)
{
	/* Texture and animation update. */
	updateFacing(playerPosition.x);
	updateAnimation();
	m_sprite.updateTexture(dt, m_facingRight);

	/* Position update. */
	m_body.updatePosition(dt);
	/* BB update. */
	m_BB.updateBB(m_body.getPosition() + gameData::bbOffsetPosition);

	/* State update. */
	attackHandler(dt, projectileSys, playerPosition);
	jumpMoveHandler(dt, projectileSys, playerPosition.x, cameraPos);
	deathHandler();
}

void Cat::resetCat()
{
	m_body.setPosition(gameData::catPosition);
	m_body.setVelocity(glm::vec2(0.0f));
	m_state = State::Idle;
	m_prevState = State::Walk;
	m_lives = gameData::catLives;
	m_facingRight = false;
	m_canMove = true;
	m_onGround = true;
	m_attackTimer = 0.0f;
	m_energyBallFired = false;
	m_blastTimer = 0.0f;
	m_blastCount = 0;
}


void Cat::setAnimation(int start, int end, int line, float fps)
{
	m_sprite.setAnimation(start, end, line, fps);
}

void Cat::updateAnimation()
{
	/* Only set a new animation when the state changes. */
	if (m_state != m_prevState)
	{
		/* operator[] for unordered_map gives error because playerAnimations is declared const.
		 * So the use of the function at() solves this issue. */
		const AnimationData& anim = gameData::catAnimations.at(m_state);
		m_sprite.setAnimation(anim.startFrame, anim.endFrame, anim.animationLine, anim.fps);
		m_prevState = m_state;
	}
}