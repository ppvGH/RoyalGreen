#include "callbacks.h"
#include <imgui_impl_glfw.h>
#include <iostream>

void Callbacks::initCallbacks(GLFWwindow* window)
{
	// Framebuffer
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Key inputs
	glfwSetKeyCallback(window, keyCallback);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);

}

void Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	// ESC key to close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}

void Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}


