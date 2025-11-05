#pragma once

#include "../../graphics/camera.h"
#include "../../graphics/framebuffer.h"
#include "../../core/commonGL.h"
#include "../../core/action_map.h"
#include "arcade.h"
#include "room.h"


class Scene
{
public:



	/* Scene constructor with window dimensions. */
	Scene(int width, int height);


	void update();

	bool isGameOn() const { return m_gameOn; }

	// #########################################################################
	// ############################  Input system  #############################
	// #########################################################################

	/* System input: it is made impossible for 3D and 2D input to be true at the same time (alternate switches). */
	/* TODO: RIDUCIBILI AD UN METODO CHE ALTERNA 2D e 3D, quindi invece di 6 metodi -> 3 metodi. */

	bool isInput3DEnabled() const { return m_input3D; }
	bool isInput2DEnabled() const { return m_input2D; }

	void setInput3D(bool input3D) { if (input3D) m_input2D = false; m_input3D = input3D; }
	void setInput2D(bool input2D) { if (input2D) m_input3D = false; m_input2D = input2D; }

	void input3DHandler(GLFWwindow* window, const ActionMap& actionMap3D);

	bool picking() const;


	// #########################################################################
	// ##########################  Objects methods  ############################
	// #########################################################################


	/* Wrappers for Arcade methods. */

	bool isDisplayOn() const { return m_arcade.isDisplayOn(); }

	void switchArcadeScreen() { m_arcade.screenSwitch(); }

	/* Wrapper for Arcade::setScreen method. */
	void setArcadeScreenTex(const Texture& tex) { m_arcade.setScreen(tex); }



	// #########################################################################
	// ###########################  Camera methods  ############################
	// #########################################################################

	/* 3D camera input handler. */
	void cam3DinputHandler(GLFWwindow* window, const ActionMap& actionMap3D	);

	/* Getter for the animationIsOn boolean. */
	bool isAnyAnimationOn() const { return m_animInIsOn || m_animOutIsOn; }
	bool isAnimationInOn() const { return m_animInIsOn; }
	bool isAnimationOutOn() const { return m_animOutIsOn; }
	
	/* Camera animation: from the current cam position to a target point, from which
	 * the camera points to the center of the display.
	 * TODO: maybe BOOLEAN return type is not needed anymore. */
	bool cameraInAnimation();

	bool cameraOutAnimation();

	/* Camera getters. */

	Camera& getCam3D() { return m_cam3D; }			// writeable
	const Camera& getCam3D() const { return m_cam3D; }	// read-only

	// #########################################################################
	// #################################  Aim  #################################
	// #########################################################################

	/* Getter for sight state. //TODO: INUTILIZZATI, MAGARI CANCELLARE*/
	bool aimIsOn() const { return m_aimIsOn; }	
	/* Switch ON for sight state. */
	void switchAimOn() { m_aimIsOn = true; }
	/* Switch OFF for sight state. */
	void switchAimOff() { m_aimIsOn = false; }


	// #########################################################################
	// #################################  GUI  #################################
	// #########################################################################

	bool isArcadeMenuOpen() const { return m_arcadeMenuOpen; }
	void openArcadeMenu() { m_arcadeMenuOpen = true; }
	void closeArcadeMenu() { m_arcadeMenuOpen = false; }

	// #########################################################################
	// ##############################  Rendering  ##############################
	// #########################################################################

	/* Lighting pass. */
	void drawScene();

	/* Depth pass. */
	void depthPass();
	

private:
	
	/* Sizes of window. */
	int m_width, m_height;

	/* Input system flags. */
	bool m_input3D = true;
	bool m_input2D = false;

	/* Game variable. */
	bool m_gameOn = false;

	/* Camera stuff. */
	Camera m_cam3D;

	/* Callback state. */
	/* If cursorCentered = false, cursor needs to be centered. */
	bool m_cursorCentered;
	double m_lastX;
	double m_lastY;

	/* Scene objects.*/
	Room m_room;
	Arcade m_arcade;
	Model m_pool;
	Model m_mainLamp;
	Model m_tableLamp;
	Model m_tmp;

	/* Lights position and data. */
	glm::vec3 m_mainPointLightPos;
	glm::vec3 m_tablePointLightPos;

	// glm::vec3 m_spotLightPos;
	// glm::vec3 m_spotLightTarget;
	// glm::mat4 m_spotLightSpace;

	/* Framebuffers for scene lights. */
	Framebuffer m_mainDepthCubeFBO;
	Framebuffer m_tableDepthCubeFBO;
	// Framebuffer m_depthSpotFBO;

	/* Depth pass for a single point light. fbo is a 3D depth framebuffer. */
	void pointLightShadowMap(glm::vec3 lightPos, Framebuffer& fbo, float nearPlane, float farPlane);

	/* Depth pass for a single spot light. spotLightSpace fbo is a 2D depth framebuffer. fovy is in degrees. */
	void spotLightShadowMap(glm::vec3 lightPos, glm::vec3 lightTarget, glm::mat4& spotLightSpace, Framebuffer& fbo, float fovy, float aspect, float nearPlane, float farPlane);

	/* Draw all models with a given shader. */
	void drawModels(const std::string& shaderName);

	/* Returns false when an animation is finished. */
	bool m_animInIsOn = false;
	bool m_animOutIsOn = false;

	/* Activates second part of animation. When it is false the first part of animation is active. */
	bool m_animSecondPart = false;

	/* Animation auxiliary variable. */
	glm::vec3 m_startPos, m_startFront;
	float m_animStartTime = 0.0f;
	float m_animMidTime = 0.0f;
	float m_animFirstPartDuration;
	float m_animSecondPartDuration;

	/* Initialization of scene (loading models and setting modelMatrices). */
	void initScene();


	/* Sight VAO. */
	unsigned int m_aimVAO;
	/* Control variable for sight. */
	bool m_aimIsOn;
	/* Initialization of sight. */
	void initAim();
	/* Draw sight at the center of the screen. */
	void drawAim(Shader& shader) const;



	/* GUI variables and methods. */
	bool m_arcadeMenuOpen = false;


	
};

