#include "action_map.h"

ActionMap::ActionMap(const InputManager& inputManager):
	m_inputManager(inputManager)
{
}

void ActionMap::bind(const Action& action, int key)
{
	m_bindings[action] = key;
}

/* Looks up the action in the bindings map.
 * If found, calls the corresponding InputManager method
 * with the mapped button[key] value and returns the result. */

bool ActionMap::justStarted(const Action& action) const
{
	
	auto it = m_bindings.find(action);
	return it != m_bindings.end() && m_inputManager.isPressed(it->second);
}

bool ActionMap::ongoing(const Action& action) const
{
	auto it = m_bindings.find(action);
	return it != m_bindings.end() && m_inputManager.isHeld(it->second);
}

bool ActionMap::justFinished(const Action& action) const
{
	auto it = m_bindings.find(action);
	return it != m_bindings.end() && m_inputManager.isReleased(it->second);
}
