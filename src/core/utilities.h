#pragma once
#include "commonGL.h"
#include <glm/glm.hpp>

class Scene;	// scene.h is included in utilities.cpp


/* Initialize GLFW and creates a window with same ratio as the user display.
 * Default scale correspond to full screen. */
GLFWwindow* initGLFWwindow(const char* title = "", float scale = 1);

/* Initialize GLFW and creates a window with custom width and height. */
GLFWwindow* initGLFWwindow(int width, int height, const char* title);

/* Initializes GLAD and destroys window if an error occurs */
bool initGLAD(GLFWwindow* window);

/* Print glm::vec3. */
void printVec3(const glm::vec3& vec);
/* Print glm::vec4. */
void printVec4(const glm::vec4& vec);

/* Computes dt = glfwGetTime() - lastFrame and updates lastFrame. */
float getDeltaTime(float& lastFrame);