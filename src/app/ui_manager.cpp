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

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

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

void UIManager::updateInput(const InputManager& inputManager)
{
	ImGuiIO& io = ImGui::GetIO();

	// frecce e invio per navigazione
	io.AddKeyEvent(ImGuiKey_UpArrow, inputManager.isHeld(GLFW_KEY_UP));
	io.AddKeyEvent(ImGuiKey_DownArrow, inputManager.isHeld(GLFW_KEY_DOWN));
	io.AddKeyEvent(ImGuiKey_LeftArrow, inputManager.isHeld(GLFW_KEY_LEFT));
	io.AddKeyEvent(ImGuiKey_RightArrow, inputManager.isHeld(GLFW_KEY_RIGHT));
	io.AddKeyEvent(ImGuiKey_Enter, inputManager.isPressed(GLFW_KEY_ENTER));
	io.AddKeyEvent(ImGuiKey_Escape, inputManager.isPressed(GLFW_KEY_ESCAPE));

}

void UIManager::beginFrame()
{
	if (!m_initialized) return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_isAnyMenuOpen = false;
}

void UIManager::drawUI(Scene& scene, int width, int height)
{
	if (scene.isArcadeMenuOpen())
	{
		scene.setInput3D(false);
		drawCabinetMenu(scene, width, height);
		m_isAnyMenuOpen = true;
	}

	drawDebugOverlay(scene, width, height);
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

	// se il menu è aperto, abilita cursore e disabilita input camera
	GLFWwindow* window = glfwGetCurrentContext();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	ImGui::Begin("Arcade", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize);

	ImGui::Text("Start the game?");
	ImGui::Separator();

	if (ImGui::Button("Play", ImVec2(120, 0)))
	{
		//scene.setInput3D(true);
		scene.cameraInAnimation();
		scene.closeArcadeMenu();
	}
	if (ImGui::Button("Exit", ImVec2(120, 0))) 
	{
		//scene.setInput3D(true);
		scene.closeArcadeMenu();
	}

	// se il menu si chiude, riabilita il cursore disabilitato per la camera
	if (!scene.isArcadeMenuOpen())
	{
		scene.setInput3D(true);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	ImGui::End();
}

void UIManager::drawPauseMenu(Scene& scene, int width, int height)
{
}

void UIManager::drawDebugOverlay(Scene& scene, int width, int height)
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.3f);

	ImGui::Begin("Debug", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)",
		scene.getCam3D().getPosition().x,
		scene.getCam3D().getPosition().y,
		scene.getCam3D().getPosition().z);

	ImGui::End();
}
