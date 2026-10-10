#pragma once

/** @file
 * Frame-sampled input state. Poll GLFW events before update(), then query actions.
 * Pressed and Released last one sample; Held and Up are level states.
 */

#include <unordered_map>
#include "commonGL.h"

enum class KeyState { Pressed, Released, Held, Up };


class InputManager
{
public:
	InputManager() = default;

	/** Register a GLFW keyboard or mouse code; re-registration resets it to Up. */
	void registerKey(int key);

	/** Sample registered codes once per frame using a valid GLFW window.
	 * Down transitions Up/Released -> Pressed -> Held; up transitions
	 * Pressed/Held -> Released -> Up.
	 * Very short taps between samples can be missed, so this is a polling API.
	 */
	void update(GLFWwindow* window);

	/* Unknown codes query false. Held excludes the initial Pressed sample. */

	bool isPressed(int key) const;
	bool isHeld(int key) const;
	/** True only on the first up sample after press/hold; false at registration. */
	bool isReleased(int key) const;

	/** GLFW mouse codes occupy [GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_LAST]. */
	bool isMouseButton(int key);


private:
	/* Int: the GLFW_KEY_* ; KeyState: its state. */
	std::unordered_map<int, KeyState> m_keyStates;
};