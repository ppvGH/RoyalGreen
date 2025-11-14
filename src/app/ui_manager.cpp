#include "ui_manager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

UIManager::UIManager() :
	m_initialized(false),
	m_isAnyMenuOpen(false)
{
}

void UIManager::init(GLFWwindow* window)
{
	if (m_initialized) return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init("#version 130");

	m_initialized = true;
}

void UIManager::shutdown()
{
	if (!m_initialized) return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_initialized = false;
}



void UIManager::beginFrame()
{
	if (!m_initialized) return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_isAnyMenuOpen = false;
}

void UIManager::drawUI(Scene& scene, Game& game, int width, int height)
{
	if (scene.isArcadeMenuOpen())
	{
		scene.setInput3D(false);
		drawCabinetMenu(scene, width, height);
		m_isAnyMenuOpen = true;
	}

	if (game.isGameMenuOpen())
	{
		scene.setInput2D(false);
		drawGameMenu(scene, game, width, height);
		m_isAnyMenuOpen = true;
	}

	if(!scene.isDisplayOn())
	{
		if (scene.isHelpOverlayOn()) drawHelpOverlay3D(scene, width, height);
	}
	else
	{
		if (game.isHelpOverlayOn()) drawHelpOverlay2D(game, width, height);
	}

	drawDebugOverlay(scene, game, width, height);
}

void UIManager::render()
{
	if (!m_initialized) return;

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::drawCabinetMenu(Scene& scene, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(width * 0.5f, height * 0.5f),
		ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Arcade", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("Start the game?");
	ImGui::Separator();

	if (ImGui::Button("Play", ImVec2(120, 0)))
	{
		if (!scene.isDisplayOn()) scene.switchArcadeScreen();
		scene.cameraInAnimation();
		scene.closeArcadeMenu();
	}
	if (ImGui::Button("Exit", ImVec2(120, 0)))
	{
		scene.closeArcadeMenu();
		scene.setInput3D(true);
	}

	// Needed for both buttons, because input3D can move camera during animation. 
	// TODO: maybe to avoid this problem it is needed to detach camera movement control from input control
	//if (!scene.isArcadeMenuOpen()) scene.setInput3D(true);

	ImGui::End();
}

void UIManager::drawGameMenu(Scene& scene, Game& game, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(width * 0.5f, height * 0.5f),
		ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Game Menu", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("Select one option.");
	ImGui::Separator();

	if (ImGui::Button("Return to game", ImVec2(120, 0)))
	{
		game.closeGameMenu();
		scene.setInput2D(true);
	}
	if (ImGui::Button("Reset game", ImVec2(120, 0)))
	{
		game.closeGameMenu();
		game.resetGame();
		scene.setInput2D(true);
	}
	if (ImGui::Button("Exit game", ImVec2(120, 0)))
	{
		game.closeGameMenu();
		scene.switchArcadeScreen();
		game.resetGame();
		scene.cameraOutAnimation();
	}

	ImGui::End();

}

void UIManager::drawHelpOverlay3D(Scene& scene, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("Scene helper.", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse);

	ImGui::Text("WASD: Move camera.");
	ImGui::Text("F: Fix camera height.");
	ImGui::Text("Click on arcade: play!");
	ImGui::Text("H: Show/Hide the helper.");
	ImGui::Text("T: Show/Hide the aim cross.");

	ImGui::End();

}

void UIManager::drawHelpOverlay2D(Game& game, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(10, 50), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("Game helper.", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse);

	ImGui::Text("A/D: Move left/right.");
	ImGui::Text("W: Jump.");
	ImGui::Text("S: Shoot.");
	ImGui::Text("M: Open menu.");
	ImGui::Text("H: Show/Hide the helper.");

	ImGui::End();

}


void UIManager::drawDebugOverlay(Scene& scene, Game& game, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("Debug", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	/*if(!scene.isDisplayOn())
	{
		ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)",
			scene.getCam3D().getPosition().x,
			scene.getCam3D().getPosition().y,
			scene.getCam3D().getPosition().z);
	}*/
	//if(scene.isDisplayOn())
	//{
	//	//ImGui::Text("Camera Pos : %.2f", game.getCamPos());
	//	//ImGui::Text("Cat Lives : %d", game.getCatLives());
	//}

	ImGui::End();
}
