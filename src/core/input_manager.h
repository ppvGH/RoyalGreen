#pragma once

#include <unordered_map>
#include "commonGL.h"

enum class KeyState { Pressed, Released, Held };


class InputManager
{
public:
	InputManager() = default;

	/* Insert a key into the keyStates map. */
	void registerKey(int key);

	/* Updates all keys states. */
	void update(GLFWwindow* window);

	/* Info about a certain state of a given key.
	 * If key exists and its state is (P,H,R) returns true.
	 * N.B. the Held state can suddenly appear as a single fast tap covers 4 frames at least. */

	bool isPressed(int key) const;
	bool isHeld(int key) const;
	bool isReleased(int key) const;


private:
	/* Int: the GLFW_KEY_* ; KeyState: its state. */
	std::unordered_map<int, KeyState> m_keyStates;
};