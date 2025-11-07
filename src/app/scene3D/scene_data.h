#pragma once

#include <string>
#include <glm/glm.hpp>

namespace sceneData
{

	/* Shaders data. */
	/* Names*/
	inline const std::string blinnPhongShaderName = "blinnPhong";
	inline const std::string pointLightDepthShaderName = "pointDepth";
	inline const std::string spotLightDepthShaderName = "spotDepth";
	inline const std::string CRTShaderName = "CRT";
	inline const std::string aimShaderName = "aim";
	inline const std::string gameShaderName = "game";

	/* Global shaders settings. */
	inline const std::string displayShader = CRTShaderName; // blinnPhongShaderName; // 
	inline const std::string renderShader = blinnPhongShaderName;

	/* Arcade model data. */
	inline const std::string meshScreenName = "display";
	inline const std::string meshYMinFace = "cabinet_y_min_face";
	inline const std::string meshPowerButtonName = "power_button_power_button";
	inline const glm::vec3 arcadeModelPositionShift = glm::vec3(-2.9f, 0.02f, -0.8f); // 0.02 is to lift the model a tiny bit.
	inline const float powerButtonEmission = 10.0f;

	/* Room model data.*/
	inline const std::string meshFloorName = "floor";
	inline const std::string matFloorName = "floor";
	inline const float floorTileFactor = 16.0f; // 10/16 = 0.667 (size of the tile)

	/* Main lamp model data. */
	inline const glm::vec3 mainLampModelPositionShift = glm::vec3(0.0f, 2.95f, 0.0f);

	/* Table lamp model data. */
	inline const glm::vec3 tableLampModelPositionShift = glm::vec3(0.0f, 1.9f, 0.0f);

	/* Pool model data. */
	inline const glm::vec3 poolModelPositionShift = glm::vec3(0.5f, 0.0f, 1.5f);

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
	inline const float inAnimationFirstPartDuration = 2.0f; // 0.01f; // 
	inline const float inAnimationSecondPartDuration = 1.0f; // 0.01f; // 

	/* Geometry data. */
	inline const float intersectionEpsilon = 1e-6f;

	/* Framebuffer data. */
	inline const char FBOtypeColor = 'c';
	inline const char FBOtypeDepth = 'd';
	inline const char FBOtypeCubeDepth = 'm';

	/* Shadow data. */
	inline const int shadowWidth = 1024;
	inline const int shadowHeight = 1024;

	/* Main point light data. */
	inline const float mainPointLightNearPlane = 1.0f;
	inline const float mainPointLightFarPlane = 10.0f;	// covers entire room
	inline const float mainPointLightInvFarPlane = 1.0 / mainPointLightFarPlane;

	/* Table point light data. */
	inline const float tableLampHeightFix = 0.00f;	// table PL height fix: just above the lightbulb to shrink(>0) or enlarge(<0) the light cone projected
	inline const float tableLightYoffset = 0.05f;	// value for the shader to lighten the internal part of lampshade

	inline const float tablePointLightNearPlane = 0.1f;
	inline const float tablePointLightFarPlane = 5.0f;	
	inline const float tablePointLightInvFarPlane = 1.0 / tablePointLightFarPlane;

	inline const float tableCosInnerAngle = glm::cos(glm::radians(35.0f));		// if heightFix >0 --> lower this || if heightFix < 0 --> up this
	inline const float tableCosOuterAngle = glm::cos(glm::radians(45.0f));		// if heightFix >0 --> lower this || if heightFix < 0 --> up this
	inline const float tableInvCosDelta = 1 / (tableCosInnerAngle - tableCosOuterAngle);

	


	/* Old data. */
	inline const float arcadeAnimationSpeed = 0.005f;
}