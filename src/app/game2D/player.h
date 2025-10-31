#pragma once

#include "sprite.h"
#include "game_obj.h"
#include "../../core/action_map.h"
#include "state.h"

class ProjectileManager;	// fwd decl

class Player
{
public:
	Player(const std::string& texName, glm::vec2 position, glm::vec2 size, glm::vec2 velocity);

	/* Input manager. Sets states. */
	void inputHandler(const ActionMap& actionMap2D, float dt, ProjectileManager& projectilesSys);

	/* Update graphics (sprite) position, velocity (body) and behaviour from states. */
	void update(float dt, ProjectileManager& projectilesSys);

	/* Position and size are entangled with its GameObj body, so they would be meaningless as external inputs. */
	void render(const SpriteRenderer& spriteRenderer, Shader& shader) const;

	/* Reset player to initial state. */
	void resetPlayer();


	/* Getter, setter. */

	glm::vec2 getPosition() const { return m_body.getPosition(); }
	glm::vec2 getSize() const { return m_body.getSize(); }
	const Texture& getTex() const { return m_sprite.getTex(); }

	//void setPosition(const glm::vec2& pos) { m_body.setPosition(pos); }
	//void setVelocity(const glm::vec2& vel) { m_body.setVelocity(vel); }


private:
	GameObj m_body;
	Sprite m_sprite;
	/* Current state. */
	State m_state;
	/* Previous state. Set in updateAnimation(). */
	State m_prevState;

	/* Sets animation for the sprite. */
	void setAnimation(int start, int end, int line, float fps);
	/* Separated from update to keep it clean.
	 * Utilizes a map to set animation from gameData namespace.
	 * Sets prevState from actual state. */
	void updateAnimation();

	/* Game logic variables and methods.*/

	bool m_facingRight = true;
	bool m_onGround = true;	// false while jumping
	bool m_canMove = true;	// false while shooting
	bool m_shotFired = false;	// true after shot, reset false when attack animation ends

	bool isIdle(const ActionMap& actionMap2D);
	void atIdle();
	void walk(bool facingRight);
	void setJump();
	void jumpHandler(float dt);
	void setAttack();
	void attackHandler(ProjectileManager& projectilesSys);

	/* Emits projectile object. */
	void shootArrow(ProjectileManager& projectilesSys);

	/* Prints state when there is a variation.*/
	void printNewState(const ActionMap& actionMap2D);
};