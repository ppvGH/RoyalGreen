#pragma once

#include "../graphics/camera.h"
#include "../core/commonGL.h"
#include "../core/action_map.h"
#include "arcade.h"


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

	void setSceneInput(bool sceneSwitch) { m_input3D = sceneSwitch; m_input2D = !sceneSwitch; }

	void input3DHandler(GLFWwindow* window, const ActionMap& actionMap3D);


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
	bool getAnimationIsOn() const { return m_animationIsOn; }
	/* Camera animation: from the current cam position to a target point, from which
	 * the camera points to the center of the display. TODO: could be improved using a
	 * track like a Bezier curve or making the path not intersect with the arcade mesh. 
	 * (bounding box and ray picking mechanism needed). */
	bool cameraAnimation();

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

	/* Draw the 3D scene. */
	void drawScene() const;
	// 3D camera input handler
	void cam3DinputHandler(GLFWwindow* window, const ActionMap& actionMap3D	);
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
	Model m_room;

	/* Returns false when the animation is finished. */
	bool m_animationIsOn = false;
	/* Activates last part of animation. When it is false the first part of animation is active. */
	bool m_animLastPart = false;
	/* Animation auxiliary variable. */
	glm::vec3 m_startPos, m_startFront;

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

};

