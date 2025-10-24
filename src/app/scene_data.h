#pragma once

#include <string>
#include <glm/glm.hpp>

namespace sceneData
{
	/* Arcade model data. */
	inline const std::string meshScreenName = "display";
	inline const std::string matScreenName = "display";
	inline const std::string matPowerButtonName = "power_button_power_button";
	inline const glm::vec3 arcadeModelPositionShift = glm::vec3(-2.4f, 0.02f, 0.0f); // 0.02 is to lift the model a tiny bit.

	/* Room model data.*/
	inline const std::string meshFloorName = "floor";
	inline const std::string matFloorName = "floor";
	inline const float floorTileFactor = 16.0f; // 10/16 = 0.667 (size of the tile)

	/* Lamp model data. */
	inline const glm::vec3 lampModelPositionShift = glm::vec3(0.0f, 2.5f, 0.0f);


	/* Pool model data. */
	inline const glm::vec3 poolModelPositionShift = glm::vec3(0.0f, 0.0f, 2.0f);

	/* Camera data. */
	inline const glm::vec3 cameraLightPosition = glm::vec3(0.0f, 2.5f, 0.0f);
	inline const float cameraAltitude = 1.7f;
	inline const glm::vec3 cameraStartingPosition = glm::vec3(2.0f, cameraAltitude, 0.0f);
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

	/* Framebuffer data. */
	inline const char FBOtypeColor = 'c';
	inline const char FBOtypeDepth = 'd';
	inline const char FBOtypeCubeDepth = 'm';

	/* Shadow data. */
	inline const int shadowWidth = 1024;
	inline const int shadowHeight = 1024;

	/* Point light data. */
	inline const float pointLightFovy = 90.0f;
	inline const float pointLightAspect = 1.0f;	// square frustum
	inline const float pointLightNearPlane = 1.0f;
	inline const float pointLightFarPlane = 10.0f;	// covers entire room
	inline const float pointLightInvFarPlane = 1.0 / pointLightFarPlane;

	/* Shaders data. */

	/* Names*/
	inline const std::string blinnPhongShaderName = "blinnPhong";
	inline const std::string pointLightDepthShaderName = "pointDepth";
	inline const std::string spotLightDepthShaderName = "spotDepth";
	inline const std::string CRTShaderName = "CRT";
	inline const std::string aimShaderName = "aim";
	inline const std::string gameShaderName = "game";


	/* Old data. */
	inline const float arcadeAnimationSpeed = 0.005f;
}