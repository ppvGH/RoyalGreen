#pragma once

#include "sprite.h"
#include "game_obj.h"
#include "../../core/action_map.h"
#include "state.h"

class Player
{
public:
	Player(const std::string texName, 
		glm::vec2 position,
		glm::vec2 size,
		float walkSpeed);


	/* Input manager. */
	void inputHandler(const ActionMap& actionMap2D, float dt);



	/* Update graphics (sprite) and logic (body). */
	void update(float dt);

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
	/* Previous state. */
	State m_prevState;

	/* Sets animation for the sprite. */
	void setAnimation(int start, int end, int line, float fps);
	/* Separated from update to keep it clean. */
	void updateAnimation();

	/* Game logic variables and methods.*/

	bool m_facingRight = true;
	//bool m_isJumping = false;
	bool m_canWalk = true;	// false while jumping / attacking / dying (and after death ofc)

	bool isIdle(const ActionMap& actionMap2D);
	void atIdle();
	void walk(bool facingRight);
	void jump(float dt);
};