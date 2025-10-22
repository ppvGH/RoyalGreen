#pragma once

#include "../graphics/camera.h"
#include "../core/commonGL.h"
#include "../core/action_map.h"
#include "arcade.h"
#include "room.h"


class Scene
{
public:



	/* Scene constructor with window dimensions. */
	Scene(int width, int height);

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

	void switchArcadeScreen() { m_arcade.screenSwitch(); }

	void setArcadeScreenTex(const Texture& tex) { m_arcade.setScreen(tex); }



	// #########################################################################
	// ###########################  Camera methods  ############################
	// #########################################################################

	/* Initialize camera3D. */
	void initCam3D() const;

	/* Getter for the animationIsOn boolean. */
	bool isAnyAnimationOn() const { return m_animInIsOn || m_animOutIsOn; }
	bool isAnimationInOn() const { return m_animInIsOn; }
	bool isAnimationOutOn() const { return m_animOutIsOn; }
	
	/* Camera animation: from the current cam position to a target point, from which
	 * the camera points to the center of the display.
	 * TODO: maybe BOOLEAN return type is not needed anymore. */
	bool cameraInAnimation();

	bool cameraOutAnimation();

	/* Camera methods. */

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

	void drawAim() const;

	/* Draw the 3D scene. */
	void drawScene() const;
	// 3D camera input handler
	void cam3DinputHandler(GLFWwindow* window, const ActionMap& actionMap3D	);

	// #########################################################################
	// #################################  GUI  #################################
	// #########################################################################

	bool isArcadeMenuOpen() const { return m_arcadeMenuOpen; }
	void openArcadeMenu() { m_arcadeMenuOpen = true; }
	void closeArcadeMenu() { m_arcadeMenuOpen = false; }

private:
	
	/* Sizes of window. */
	int m_width, m_height;

	/* Input system flags. */
	bool m_input3D = true;
	bool m_input2D = false;

	/* Camera stuff. */
	Camera m_cam3D;
	//bool m_useCam3D;


	/* Callback state. */
	/* If cursorCentered = false, cursor needs to be centered. */
	bool m_cursorCentered;
	double m_lastX;
	double m_lastY;

	/* Scene objects and their position offset.
	 * Offsets need to be updated after updating the model uniform matrix in the shader. */
	Arcade m_arcade;
	Room m_room;
	Model m_lamp;
	Model m_pool;

	/* Returns false when an animation is finished. */
	bool m_animInIsOn = false, m_animOutIsOn = false;
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





// #################################################################################################################
// ##############################################   DESCARDED METHODS   ############################################
// #################################################################################################################


	/* Camera animation: from the current cam position to a target point, from which
	 * the camera points to the center of the display. TODO: could be improved using a
	 * track like a Bezier curve or making the path not intersect with the arcade mesh.
	 * (bounding box and ray picking mechanism needed). */
	/*bool cameraAnimationOLD();*/
};

