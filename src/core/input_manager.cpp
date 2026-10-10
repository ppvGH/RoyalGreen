#include "input_manager.h"

void InputManager::registerKey(int key)
{
	// Registration has no synthetic release event.
	m_keyStates[key] = KeyState::Up;
}

void InputManager::update(GLFWwindow* window)
{ 
	for (auto& [key, state] : m_keyStates)
	{
        // Polling can miss taps occurring entirely between two samples.

		int item = isMouseButton(key) ? glfwGetMouseButton(window, key) : glfwGetKey(window, key);

		if(item == GLFW_PRESS)
			state = (state == KeyState::Released || state == KeyState::Up) ? KeyState::Pressed : KeyState::Held;
		else
			state = (state == KeyState::Pressed || state == KeyState::Held) ? KeyState::Released : KeyState::Up;
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

bool InputManager::isMouseButton(int key)
{
	/* Left = button1, right = button2, middle = button3, ... */
	return key >= GLFW_MOUSE_BUTTON_1 && key <= GLFW_MOUSE_BUTTON_LAST;
}