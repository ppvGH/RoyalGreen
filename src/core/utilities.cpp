#include "utilities.h"

#include "../app/scene.h"

#include <iostream>
#include <cmath>

GLFWwindow* initGLFWwindow(const char* title, float scale)
{
    if (scale > 1 || scale < 0)
    {
        std::cout << "ABORT::initGLFWwindow: scale is not in range [0,1]." << std::endl;
        glfwTerminate();
        throw std::runtime_error("initGLFWwindow: scale is not in range [0,1].");
        return nullptr;
    }
    if (!glfwInit())
    {
        std::cerr << "ERROR::GLFW::glfwInit failure." << std::endl;
        return nullptr;
    }

    /* Retrieving monitor info. */
    int winWidth = 200, winHeight = 200;
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if (primary)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        if (mode)
        {
            winWidth = scale *  mode->width;
            winHeight = scale * mode->height;
        }
    }
    // OpenGL version 3.3 core + doublebuffer
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Window management
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "ERROR::GLFW::glfwCreateWindow failure." << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    return window;
}

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

void printVec3(const glm::vec3& vec)
{
    std::cout << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')' << std::endl;
}

void printVec4(const glm::vec4& vec)
{
    std::cout << '(' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ')' << std::endl;
}



float getDeltaTime(float& lastFrame)
{
    float currentFrame = static_cast<float>(glfwGetTime()); // tempo attuale in secondi
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}

