#pragma once

#include "game_obj.h"
#include "sprite.h"
#include "state.h"

#include "../../core/collision.h"

class ProjectileManager;

class Cat
{
public:
	Cat() = delete;

	Cat(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity);

	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

	/* Manages states setup with automatic inputs.*/
	void autoInput(float dt, float playerXpos);

	/* Attack manager.*/
	void attackHandler(float dt, ProjectileManager& projectileSys, glm::vec2 playerPosition);

	/* Jump Move manager. It is a jump + cast ability move. */
	void jumpMoveHandler(float dt, ProjectileManager& projectileSys, float playerXpos, float cameraPos);


	/* Manages AI, movements, animations update.*/
	void update(float dt, ProjectileManager& projectileSys, glm::vec2 playerPosition, float cameraPos);

	/* Reset initial values. */
	void resetCat();

	/* Getter. */
	glm::vec2 getPosition() const { return m_body.getPosition(); }
	State getState() const { return m_state; }
	int getLives() const { return m_lives; }
	GameObj& getGameObj() { return m_body; }
	const BoundingBox getBB() const { return m_BB; }

	/* Hit manager. */
	void hasBeenHit();


private:
	GameObj m_body;
	Sprite m_sprite;

	BoundingBox m_BB;

	State m_state;
	State m_prevState;	// used only for animation, not logic

	int m_lives = 9;

	bool m_facingRight = false;
	bool m_canMove = true;
	bool m_onGround = true;
	float m_attackTimer = 0.0f;
	bool m_setJumpMove = false;		// flag to setup jump move attack when first entering the function
	bool m_energyBallFired = false;
	bool m_blastCharge = true;		// false after blast charge by lives count, reset after every hit
	float m_blastTimer = 0.0f;
	int m_blastCount = 0;

	void incrementTimers(float dt);

	void setAttack();

	void shootEnergyBall(ProjectileManager& projectileSys, glm::vec2 playerPosition, float speed);

	void setAnimation(int start, int end, int line, float fps);

	void setJumpMove();

	void updateFacing(float playerXpos);

	void deathHandler();

	void updateAnimation();
};