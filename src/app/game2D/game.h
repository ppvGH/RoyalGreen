#pragma once

#include "framebuffer.h"
#include "sprite_renderer.h"
#include "player.h"
#include "background.h"

#include "../../core/action_map.h"

class Game 
{
public:


	Game() = delete;

	Game(int width, int height);

	void initGame();

	void input2DHandler(const ActionMap& actionMap2D, float dt);

	void update(float dt);

	void render() const;



	

	/* Getters. */
	//const Framebuffer& getFBO() const { return m_FBO; }		//TODO: NON UTILIZZATO; CANCELLARE?
	const SpriteRenderer& getSpriteRenderer() const { return m_spriteRenderer; }
	const Texture& getFBOTex() const { return m_FBO.getTex(); }

	bool isGameMenuOpen() const { return m_gameMenuOpen; }
	void closeGameMenu() { m_gameMenuOpen = false; }

private:

	/* Internal variables.*/
	float m_width, m_height;


	/* Game items. */
	Player* m_player;
	Background* m_background;

	/* Camera is just an X coordinate for now. */
	float m_cameraPosition;
	/* Limit of camera view. */
	float m_cameraMargin;
	/* Updates camera position. */
	void updateCamera();

	/* Input management. */
	bool m_gameMenuOpen = false;

	void appInputHandler(const ActionMap& actionMap2D);
	void openGameMenu() { m_gameMenuOpen = true; }



	Framebuffer m_FBO;
	SpriteRenderer m_spriteRenderer;
	Shader& m_shader;
};