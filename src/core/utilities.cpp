#include "utilities.h"

#include <iostream>

GLFWwindow* initGLFWwindow(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cerr << "ERROR::GLFW::glfwInit failure." << std::endl;
        return nullptr;
    }

    // OpenGL version 3.3 core + doublebuffer
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Window management
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "ERROR::GLFW::glfwCreateWindow failure." << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    return window;
}

bool initGLAD(GLFWwindow* window)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR::GLAD::gladLoadGLLoader failure." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();

        return false;
    }

    return true;
}
