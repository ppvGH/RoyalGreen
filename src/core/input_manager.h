#pragma once

/** @file
 * Frame-sampled input state. Poll GLFW events before update(), then query actions.
 * Released is a persistent state, not a one-frame release event.
 */

#include <unordered_map>
#include "commonGL.h"

enum class KeyState { Pressed, Released, Held };


class InputManager
{
public:
	InputManager() = default;

	/** Register a GLFW keyboard or mouse code; re-registration resets it to Released. */
	void registerKey(int key);

	/** Sample registered codes once per frame using a valid GLFW window.
	 * A press transitions Released -> Pressed -> Held; release stays Released.
	 * Very short taps between samples can be missed, so this is a polling API.
	 */
	void update(GLFWwindow* window);

	/* Info about a certain state of a given key.
	 * If key exists and its state is (P,H,R) returns true.
	 * N.B. the Held state can suddenly appear as a single fast tap covers 4 frames at least. */

	bool isPressed(int key) const;
	bool isHeld(int key) const;
	/** True for a registered key that is currently up, including before its first press. */
	bool isReleased(int key) const;

	/** GLFW mouse codes occupy [GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_LAST]. */
	bool isMouseButton(int key);


private:
	/* Int: the GLFW_KEY_* ; KeyState: its state. */
	std::unordered_map<int, KeyState> m_keyStates;
};