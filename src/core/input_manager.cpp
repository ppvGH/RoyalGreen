#include "input_manager.h"

void InputManager::registerKey(int key)
{
	/* Init keys to Released state. */
	m_keyStates[key] = KeyState::Released;
}

void InputManager::update(GLFWwindow* window)
{ 
	for (auto& [key, state] : m_keyStates)
	{
		/* glfwGetKey returns the last state reported for the specified key to the specified window.
		 * States can be only GLFW_PRESS or GLFW_RELEASE. 
		 * if GLFW_PRESS: if state is released, set it to (just) pressed, if not then set it to held;
		 * else (GLFW_RELEASE) + if state is NOT released: set state to released. 
		 * N.B. the Held state can appear fast as one single tap of a key is equivalent to 4-5 frames. */
		if (glfwGetKey(window, key) == GLFW_PRESS)
			state = (state == KeyState::Released) ? KeyState::Pressed : KeyState::Held;
		else if (state != KeyState::Released)
			state = KeyState::Released;
	}
}

/* Looks up the key in the keyStates map.
 * If found, returns true if the mapped state is the requested one. */

bool InputManager::isPressed(int key) const
{
	auto it = m_keyStates.find(key);
	return it != m_keyStates.end() && it->second == KeyState::Pressed;

}

bool InputManager::isHeld(int key) const
{
	auto it = m_keyStates.find(key);
	return it != m_keyStates.end() && it->second == KeyState::Held;
}

bool InputManager::isReleased(int key) const
{
	auto it = m_keyStates.find(key);
	return it != m_keyStates.end() && it->second == KeyState::Released;
}