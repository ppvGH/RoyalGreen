#pragma once
#include <GLFW/glfw3.h>

/* Static class to organize GLFW callbacks */
class Callbacks
{
public:

    /* Starts every callback function */
    static void initCallbacks(GLFWwindow* window);

    /* Update openGL viewport when window is resized */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    /* Manages keyboard and mouse inputs */
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

