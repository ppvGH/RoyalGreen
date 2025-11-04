#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>

#include "state.h"

/* For external gameData values. Not for Animator class.
 * 1: startFrame	2: endFrame		3: animationLine	4: fps	
 */
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
	inline const glm::vec2 bbOffsetPosition = glm::vec2(33.0f, 3.0f);
	inline const glm::vec2 bbSize = glm::vec2(31.0f, 69.0f);
	
	/* Background data. */
	inline const std::string backgroundTexName = "backgroundTex";
	inline const glm::vec2 backgroundPosition = { 0.0f, 0.0f };
	inline const glm::vec2 backgroundSize = { 1422.0f, 600.0f };

	/* Player data. */
	inline const std::string playerTexName = "playerTex";
	inline const glm::vec2 playerPosition = { 0.0f, groundLevel };
	inline const glm::vec2 playerSize = { 100.0f, 100.0f };
	inline const glm::vec2 playerVelocity = { 0.0f, 0.0f };
	inline const int playerSheetRows = 6;
	inline const int playerSheetColumns = 13;
	inline const float playerWalkSpeed = 200.0f;
	inline const float playerMaxJumpSpeed = 600.0f;
	inline const std::unordered_map<State, AnimationData> playerAnimations = 
	{
		{ State::Idle,   {10, 11,  3,  5.0f} },
		{ State::Walk,   { 1,  9,  3, 10.0f} },
		{ State::Jump,   { 2,  7,  1, 10.0f} },	
		{ State::Attack, { 1,  8,  2, 25.0f} },		//25.0f deploy, 5.0 for testing
		{ State::Hit,    { 1,  6,  6, 10.0f} },
		{ State::Dead,	 { 6,  6,  6,  1.0f} }
	};

	/* Arrow data. */
	inline const int playerShootFrame = 5;
	inline const std::string arrowTexName = "arrowTex";
	inline const glm::vec2 arrowSize = glm::vec2(40.0f, 8.0f);	// arrow.png is 26x4, size should be 52x8 but 40 looks better
	inline const float arrowHeight = 0.28f;
	inline const float arrowSpeed = 600.0f; // 10.0f; // testing //

	/* Cat (enemy) data. */
	inline const std::string catTexName = "catTex";
	inline const glm::vec2 catPosition = { 500.0f, groundLevel };
	inline const glm::vec2 catSize = { 100.0f, 100.0f };
	inline const glm::vec2 catVelocity = { 0.0f, 0.0f };
	inline const int catLives = 9;
	inline const int catSheetRows = 6;
	inline const int catSheetColumns = 13;
	inline const float catWalkSpeed = 300.0f;		// higher than P1
	inline const float catMaxJumpSpeed = 1000.0f;	// higher than P1
	inline const std::unordered_map<State, AnimationData> catAnimations =
	{
		{ State::Idle,   {10, 11,  3,  5.0f} },
		{ State::Walk,   { 1,  9,  3, 10.0f} },
		{ State::Jump,   { 2,  7,  1, 10.0f} },
		{ State::Attack, { 1,  8,  2, 25.0f} },
		{ State::Hit,    { 1,  6,  6, 10.0f} },
		{ State::Dead,	 { 6,  6,  6,  1.0f} }
	};

	/* Cat attacks data. */
	inline const int catShootFrame = 5;
	inline const std::string energyBallTexName = "energyBallTex";
	inline const glm::vec2 energyBallSize = glm::vec2(20.0f);
	inline const float energyBallSpeed = 600.0f;
	inline const float blastSpeedAttenuation = 50.0f;
	inline const std::unordered_set<int> blastCharge = {1, 2, 4, 7};


	/* HUD data. */
	inline const std::string catLifeTexName = "catLifeTex";
	inline const glm::vec2 catLifeSize = glm::vec2(35.0f);
}