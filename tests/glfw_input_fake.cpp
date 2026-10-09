#include "glfw_input_fake.h"

#include <unordered_map>

namespace
{
std::unordered_map<int, int> keyReadings;
std::unordered_map<int, int> mouseReadings;
std::vector<glfw_input_fake::Poll> keyHistory;
std::vector<glfw_input_fake::Poll> mouseHistory;
}

namespace glfw_input_fake
{
void reset()
{
    keyReadings.clear();
    mouseReadings.clear();
    keyHistory.clear();
    mouseHistory.clear();
}

void setKey(int key, int state) { keyReadings[key] = state; }
void setMouseButton(int button, int state) { mouseReadings[button] = state; }
const std::vector<Poll>& keyPolls() { return keyHistory; }
const std::vector<Poll>& mouseButtonPolls() { return mouseHistory; }
} // namespace glfw_input_fake

/**
 * @brief Replace only the keyboard poll used by production InputManager.
 * @details The window is recorded but never dereferenced; missing readings
 * represent released controls. The real GLFW header checks the function ABI.
 */
extern "C" GLFWAPI int glfwGetKey(GLFWwindow* window, int key)
{
    keyHistory.push_back({window, key});
    const auto reading = keyReadings.find(key);
    return reading == keyReadings.end() ? GLFW_RELEASE : reading->second;
}

/** @brief Keep mouse polling separate so choosing the wrong GLFW API is observable. */
extern "C" GLFWAPI int glfwGetMouseButton(GLFWwindow* window, int button)
{
    mouseHistory.push_back({window, button});
    const auto reading = mouseReadings.find(button);
    return reading == mouseReadings.end() ? GLFW_RELEASE : reading->second;
}
