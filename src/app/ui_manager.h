#pragma once

#include "../core/commonGL.h"
#include "../core/input_manager.h"
#include "scene.h"


class UIManager
{
public:
	UIManager();

	void init(GLFWwindow* window);
	void shutdown();

	void updateInput(const InputManager& inputManager);

	void beginFrame();
	void drawUI(Scene& scene, int width, int height);
	void render();



	bool isMenuOpen() const { return m_isAnyMenuOpen; }

private:
	bool m_initialized = false;
	bool m_isAnyMenuOpen = false;


	void drawCabinetMenu(Scene& scene, int width, int height);
	void drawPauseMenu(Scene& scene, int width, int height);
	void drawDebugOverlay(Scene& scene, int width, int height);
};