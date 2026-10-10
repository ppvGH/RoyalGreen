#pragma once
#include "commonGL.h"
#include <glm/glm.hpp>

class Scene;


/* Initialize GLFW and creates a window with same ratio as the user display.
 * Scale must be finite in (0,1]; GLFW/window failures return null. */
GLFWwindow* initGLFWwindow(const char* title = "", float scale = 1);

/* Initialize GLFW and creates a window with custom width and height. */
GLFWwindow* initGLFWwindow(int width, int height, const char* title);

/* Load GL functions with a current context; false for null/failure.
 * Window destruction and GLFW termination remain the caller's responsibility. */
bool initGLAD(GLFWwindow* window);


/* Print glm::vec2. */
void printVec2(const glm::vec2& vec);
/* Print glm::vec3. */
void printVec3(const glm::vec3& vec);
/* Print glm::vec4. */
void printVec4(const glm::vec4& vec);
/* Print glm::mat4. */
void printMat4(const glm::mat4& mat);

/* Computes dt = glfwGetTime() - lastFrame and updates lastFrame. */
float getDeltaTime(float& lastFrame);