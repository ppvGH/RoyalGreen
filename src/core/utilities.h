#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* Checks for erros in GLFW initialization and returns a GLFWwindow pointer */
GLFWwindow* initGLFWwindow(int width, int height, const char* title);

/* Initializes GLAD and destroys window if an error occurs */
bool initGLAD(GLFWwindow* window);