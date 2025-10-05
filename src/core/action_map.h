#pragma once

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

	// Scene actions
	SwitchScreen,
	StartAnimation,
	SelectObject,

	/* 2D actions */
	// Game actions
	GameMenu,

	// Player actions
	P1MoveRight,
	P1MoveLeft,
	P1Jump,
	P1Attack


};


class ActionMap
{
public:
	/* TODO documentation*/
	ActionMap(const InputManager& inputManager);

	void bind(const Action& action, int key);

	/* Has this action just started? (Has this button just been pressed?). */
	bool justStarted(const Action& action) const;

	/* Is this action ongoing? (Is this button being held? [just for some actions] ). */
	bool ongoing(const Action& action) const;

	/* Has this action just finished? (Has this button just been released?). */
	bool justFinished(const Action& action) const;

private:
	/* TODO maybe 2 type of maps, one for 3D bindings, one for game bindings. */
	/* string: action name; int: key. */
	std::unordered_map<Action, int> m_bindings;

	const InputManager& m_inputManager;
};