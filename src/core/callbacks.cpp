#include "callbacks.h"


void Callbacks::initCallbacks(GLFWwindow* window)
{
	// Framebuffer
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Inputs
	glfwSetKeyCallback(window, keyCallback);

}

void Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ESC key to close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

}


