#pragma once

#include "../../graphics/framebuffer.h"
#include "sprite_renderer.h"
#include "background.h"
#include "projectile_manager.h"
#include "player.h"
#include "cat.h"

#include "../../core/action_map.h"

class Game 
{
public:


	Game() = delete;

	/* Initializes game variables. */
	Game(int width, int height);

	/* Resets game variables with initial parameters. */
	void resetGame();

	/* Input managing. */
	void input2DHandler(const ActionMap& actionMap2D, float dt);

	/* Logic update. */
	void update(float dt);

	/* Draws game objects into a FBO with the game shader.*/
	void render() const;



	

	/* Getters. */
	//const Framebuffer& getFBO() const { return m_FBO; }		//TODO: NON UTILIZZATO; CANCELLARE?
	const SpriteRenderer& getSpriteRenderer() const { return m_spriteRenderer; }
	const Texture& getFBOTex() const { return m_FBO.getTex(); }
	float getCamPos() const { return m_cameraPosition; }

	/* GUI. */
	bool isGameMenuOpen() const { return m_gameMenuOpen; }
	void closeGameMenu() { m_gameMenuOpen = false; }

private:

	/* Internal variables.*/
	float m_width;
	float m_height;


	/* Game items. */
	Background m_background;
	ProjectileManager m_projectilesSystem;
	Player m_player;
	Cat m_cat;

	/* Camera is just an X coordinate, corresponding to left border of the screen. */
	float m_cameraPosition;
	/* Limit of camera view. */
	float m_cameraMargin;
	/* Updates camera position. */
	void updateCamera();

	/* Input management. */
	bool m_gameMenuOpen = false;

	void appInputHandler(const ActionMap& actionMap2D);
	void openGameMenu() { m_gameMenuOpen = true; }


	/* Rendering elements. */
	Framebuffer m_FBO;
	SpriteRenderer m_spriteRenderer;
	Shader& m_shader;
};