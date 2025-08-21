#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../app/scene.h"

/* Checks for erros in GLFW initialization and returns a GLFWwindow pointer */
GLFWwindow* initGLFWwindow(int width, int height, const char* title);

/* Initializes GLAD and destroys window if an error occurs */
bool initGLAD(GLFWwindow* window);

/* Managing camera movements. Using a Scene pointer to have the possibility to receive a nullptr. */
void handleCameraInput(GLFWwindow* window, Scene* scene);