#pragma once

#include <glm/glm.hpp>

namespace sceneData
{
	/* Model data. */
	const glm::vec3 arcadeModelPositionShift = glm::vec3(-4.0f, 0.02f, 0.0f); // 0.02 is to lift the model a tiny bit.



	/* Camera data. */
	inline const glm::vec3 cameraLightPosition = glm::vec3(20.0f);
	inline const float cameraAltitude = 1.7f;
	inline const glm::vec3 cameraStartingPosition = glm::vec3(4.0f, cameraAltitude, 0.0f);
	inline const float cameraFovy = 45.0f;				// In degrees.
	inline const float cameraAspect = 16.0f / 9.0f;		// default for now is same ratio as my display. TODO: check user display ratio
	inline const float cameraNearPlane = 0.1f;
	inline const float cameraFarPlane = 20.0f;
	inline const float cameraInitialGrounded = true;
	inline const float cameraInitMouseSpeed = 0.001f;
	inline const float cameraInitKeyboardSpeed = 0.15f;


	/* Camera animation data. */
	inline const float inAnimationFirstPartDuration = 2.0f;
	inline const float inAnimationSecondPartDuration = 1.0f;


	/* Geometry data. */
	inline const float intersectionEpsilon = 1e-6f;




	/* Old data. */
	inline const float arcadeAnimationSpeed = 0.005f;
}