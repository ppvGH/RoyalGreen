#pragma once

/** @file
 * Map domain actions to registered input codes without depending on a keyboard
 * layout in game/scene logic. InputManager must outlive every referencing map.
 */

#include <unordered_map>
#include "input_manager.h"

/* TODO: SEPARATE ACTION2D e ACTION3D --> template ACTIONMAP (check unordered_map hash key stuff for compiling compatibility)*/


enum class Action
{
	/* 3D actions */
	// Camera actions
	MoveForward,
	MoveBackward,
	MoveLeft,
	MoveRight,
	SwitchCameraGrounded,

	// Scene actions
	SwitchScreen,
	StartAnimation,
	SelectObject,
	HelpOverlay3D,
	ToggleAim,


	/* 2D actions */
	// Game actions
	GameMenu,
	HelpOverlay2D,

	// Player actions
	P1MoveRight,
	P1MoveLeft,
	P1Jump,
	P1Attack


};


class ActionMap
{
public:
	/** Borrow an existing InputManager; binding does not register a key automatically. */
	ActionMap(const InputManager& inputManager);

	/** Bind/rebind an action to a GLFW code; register it in InputManager separately. */
	void bind(const Action& action, int key);

	/* Has this action just started? (Has this button just been pressed?). */
	bool justStarted(const Action& action) const;

	/* Is this action ongoing? (Is this button being held? [just for some actions] ). */
	bool ongoing(const Action& action) const;

	/** True for the release edge of the bound registered input (one sample).
	 * An unbound action or unregistered input always returns false.
	 */
	bool justFinished(const Action& action) const;

private:
	/* TODO maybe 2 type of maps, one for 3D bindings, one for game bindings. */
	/* string: action name; int: key. */
	std::unordered_map<Action, int> m_bindings;

	const InputManager& m_inputManager;
};