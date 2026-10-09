#pragma once


/* Gameplay state identifiers; string conversion below does not change state. */
enum class State
{
	Idle,
	Walk,
	Jump,
	Attack,
	Hit,
	Dead
};

/* Returns a static, null-terminated uppercase name for every named state.
 * Unnamed values of the enum's fixed underlying int type return "UNKNOWN". */
inline const char* stateToString(State state)
{
	switch (state)
	{
	case State::Idle:   return "IDLE";
	case State::Walk:   return "WALK";
	case State::Jump:   return "JUMP";
	case State::Attack: return "ATTACK";
	case State::Hit:	return "HIT";
	case State::Dead:   return "DEAD";
	default:            return "UNKNOWN";
	}
}