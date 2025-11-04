#pragma once


enum class State
{
	Idle,
	Walk,
	Jump,
	Attack,
	Hit,
	Dead
};

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