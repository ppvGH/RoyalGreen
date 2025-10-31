#pragma once

#include "game_obj.h"
#include "sprite.h"
#include "state.h"

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


private:
	GameObj m_body;
	Sprite m_sprite;

	State m_state;
	/* Set in updateAnimation method. */
	State m_prevState;

	bool m_facingRight = false;
	bool m_onGround = true;
	float m_attackTimer = 0.0f;
	bool m_energyBallFired = false;
	float m_blastTimer = 0.0f;
	int m_blastCount = 0;

	void incrementTimers(float dt);

	void setAttack();

	void shootEnergyBall(ProjectileManager& projectileSys, glm::vec2 playerPosition, float speed);

	void setAnimation(int start, int end, int line, float fps);

	void setJumpMove();

	void updateFacing(float playerXpos);

	void updateAnimation();
};