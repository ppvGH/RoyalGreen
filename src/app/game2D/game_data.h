#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "state.h"

/* Useful for external gameData values. Not useful for Animator class. */
struct AnimationData
{
	int startFrame;
	int endFrame;
	int animationLine;
	float fps;
};


namespace gameData
{
	/* Common data. */
	inline const float groundLevel = 100.0f;
	inline const float gravityAcceleration = 1200.0f;
	inline const std::string gameShaderName = "game";

	/* Player data. */

	inline const std::string playerTexName = "playerTex";
	inline const glm::vec2 playerPosition = { 0.0f, groundLevel };
	inline const glm::vec2 playerSize = { 100.0f, 100.0f };
	inline const int playerSheetRows = 6;
	inline const int playerSheetColumns = 13;
	inline const float playerWalkSpeed = 200.0f;
	inline const float playerMaxJumpSpeed = 600.0f;
	inline const std::unordered_map<State, AnimationData> playerAnimations = 
	{
		{ State::Idle,   {10, 11,  3,  5.0f} },
		{ State::Walk,   { 1,  9,  3, 10.0f} },
		{ State::Jump,   { 2,  7,  1, 10.0f} },	
		{ State::Attack, { 1,  8,  2, 20.0f} },
		{ State::Dead,   { 1,  6,  6, 10.0f} }
	};

	/* Background data. */
	inline const std::string backgroundTexName = "backgroundTex";
	inline const glm::vec2 backgroundPosition = { 0.0f, 0.0f };
	inline const glm::vec2 backgroundSize = { 1422.0f, 600.0f };

}