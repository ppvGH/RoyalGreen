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

void handleCameraInput(GLFWwindow* window, Scene* scene)
{
    if (!scene || !scene->use3D) return;

    float camSpeed = 0.025f;
    // Movement
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) scene->cam3D.moveForward(camSpeed);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) scene->cam3D.moveBackward(camSpeed);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) scene->cam3D.moveLeft(camSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) scene->cam3D.moveRight(camSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) scene->cam3D.moveUp(camSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) scene->cam3D.moveDown(camSpeed);

    // Rotation
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) scene->cam3D.turnLeft(5*camSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) scene->cam3D.turnRight(5*camSpeed);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) scene->cam3D.turnUp(5*camSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) scene->cam3D.turnDown(5*camSpeed);
}
