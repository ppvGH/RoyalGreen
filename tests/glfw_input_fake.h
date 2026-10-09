#pragma once

// Use GLFW's actual types, constants, and function declarations.
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <vector>

namespace glfw_input_fake
{
struct Poll
{
    GLFWwindow* window;
    int code;
};

/** @brief Clear readings and poll history so input scenarios cannot share state. */
void reset();
void setKey(int key, int state);
void setMouseButton(int button, int state);
const std::vector<Poll>& keyPolls();
const std::vector<Poll>& mouseButtonPolls();

/**
 * @brief Reset the fake before and after each fixture.
 * @details Cleanup also runs if a fatal Catch assertion exits a test early.
 * No GLFW initialization, window, or OpenGL context is needed.
 */
class ScopedState
{
public:
    ScopedState() { reset(); }
    ~ScopedState() { reset(); }
    ScopedState(const ScopedState&) = delete;
    ScopedState& operator=(const ScopedState&) = delete;
};
} // namespace glfw_input_fake
