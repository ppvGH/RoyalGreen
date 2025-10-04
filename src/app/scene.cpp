#include "scene.h"
#include "path.h"
#include "scene_data.h"

#include "../core/resource_manager.h"
#include <glm/gtx/transform.hpp>

#include <iostream> //for debug
#include "../core/utilities.h"

/* TODO: creare uno sgabello da mettere davanti al cabinato arcade e dare l'opzione "siediti" :
 * se selezionata la camera fa l'animazione che porta davanti al cabinato ma senza entrarci e senza bloccare gli input. 
 * blocca solo la posizione della camera. poi con il comando alzati la camera si rialza. */

/*TODO: per creare un contenitore con vari tipi di oggetti (Arcade, Room, Chair, Table, ...) potrei creare una classe base astratta
 * SceneObject con metodi virtuali getModel() = 0 e far ereditare Arcade,Room,Chair... da questa.
 * così poi in Scene posso fare std::vector<std::unique_ptr<SceneObject>> m_items; m_items.push_back(std::make_unique<Arcade>()) .. */

Scene::Scene(int width, int height) :
	m_width(width),
	m_height(height),
	m_input3D(true),
	m_input2D(false),
	m_cam3D(),
	m_cursorCentered(false),
	m_lastX((double)width * 0.5),
	m_lastY((double)height * 0.5),
	m_arcade(Path::pathModel),
	m_room(Path::pathRoom),
	m_animationIsOn(false),
	m_animSecondPart(false),
	m_startPos(glm::vec3(0.0f)),
	m_startFront(glm::vec3(0.0f)),
	m_animStartTime(0.0f),
	m_animMidTime(0.0f),
	m_animFirstPartDuration(sceneData::inAnimationFirstPartDuration),
	m_animSecondPartDuration(sceneData::inAnimationSecondPartDuration),
	m_aimVAO(0),
	m_aimIsOn(true)
{
	/* Set aspect = width/height for camera 3D. */
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	m_cam3D.setAspect(aspect);

	/* Initialize scene models and their model matrices. */
	initScene();

	/* Initialize aim assistant (sight). */
	initAim();
}

void Scene::input3DHandler(GLFWwindow* window, const ActionMap& actionMap3D)
{
	/* Disable 3D cam inputs when animation is ongoing. */
	if (!m_animationIsOn) cam3DinputHandler(window, actionMap3D);	//removed m_usecam3D from conditions if(m_useCam3D && ..)

	if (actionMap3D.justStarted(Action::StartAnimation) || m_animationIsOn)	cameraInAnimation(); 

	if (actionMap3D.justStarted(Action::SwitchScreen)) switchArcadeScreen();

	if (actionMap3D.justStarted(Action::SelectObject))
		if (picking()) openArcadeMenu();			//cameraInAnimation();		// TODO: improve here
}

bool Scene::picking() const
{
	bool hit = false;

	const Model& testModel = m_arcade.getModel();
	glm::mat4 inverseModel = glm::inverse(testModel.getModelMat());
	glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(m_cam3D.getPosition(),1.0f));
	glm::vec3 localDirect = glm::vec3(inverseModel * glm::vec4(m_cam3D.getFront(), 0.0f));			// already local

	Ray localRay(localOrigin, localDirect);

	float t;
	hit = testModel.intersectRayTriangle(localRay,t);

	if (hit) std::cout << "hit! t = " << t << std::endl;
	else std::cout << "miss!\n";

	return hit;
}

void Scene::initCam3D() const
{
	Shader& shader = ResourceManager::getShader("basic");

	shader.use();
	/* Setting shader uniforms for camera3D. */
	shader.use();
	shader.setMatrix4fv("view", 1, GL_FALSE, m_cam3D.getViewMatrix());
	shader.setMatrix4fv("proj", 1, GL_FALSE, m_cam3D.getPerspectiveProjMatrix());
	/* Setting the lights. */
	shader.setVector3f("lightPos", sceneData::cameraLightPosition);			
	shader.setVector3f("viewPos", m_cam3D.getPosition());

	/* Clear buffers. */
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}





bool Scene::cameraInAnimation()
{
	/* cursorCentered is set to false so the cursor gets centered again
	 * at the end of the animation, because cursor position can still change during animation. */
	m_cursorCentered = false;

	/* Disable fixed height. */
	m_cam3D.setGrounded(false);

	/* Display center is a point so it needs the offset of the model position in WCS;*/
	glm::vec3 displayCenter = m_arcade.getModel().getMesh("display").getCenter() + m_arcade.getModel().getWCSPosition();
	/* Display normal establishes a direction that does not need a translation. */
	glm::vec3 displayNormal = m_arcade.getModel().getMesh("display").getGlobalNormal();

	/* Intermediate point position. Let it be dC + 3*dN. */
	glm::vec3 midPos = displayCenter + 3.0f * displayNormal;
	/* The front vector should point at the display center, so it is the opposite of the display normal.*/
	glm::vec3 midFront = -displayNormal;
	/* Final point position. Let it be dC + 0.67*dN.
	 * At this point the front is already on the same direction (opposite orientation).*/
	glm::vec3 endPos = displayCenter + 0.67f * displayNormal;


	/* First time entering in the function. */
	if (!m_animationIsOn)
	{
		m_startPos = m_cam3D.getPosition();
		m_startFront = m_cam3D.getFront();

		m_aimIsOn = false;

		m_animStartTime = glfwGetTime();

		m_animSecondPart = false;

		m_animationIsOn = true;
	}

	// #################################################################################################################
	// ##################################   Animation first part: current to mid point   ###############################
	// #################################################################################################################

	if(!m_animSecondPart)
	{
		/* Time elapsed since animation started. */
		float elapsed = glfwGetTime() - m_animStartTime;

		/* Interpolation variable. 
		 * It is 0 at the beginning of the animation and 1 when elapsed equals the duration of the first part of the animation. */
		float t = glm::clamp(elapsed / m_animFirstPartDuration, 0.0f, 1.0f);

		/* Syncrhonized interpolation of position and front vectors.
		 * Using same t for both interpolation should assure their motion synchronization. */
		glm::vec3 currPos = glm::mix(m_startPos, midPos, t);
		glm::vec3 currFront = glm::normalize(glm::mix(m_startFront, midFront, t));

		/* Set camera vectors to the current ones to animate the scene.*/
		m_cam3D.setPosition(currPos);
		m_cam3D.setFront(currFront);

		/* When either one of the vector interpolated reaches its destination 
		 * set camera vectors to their target and setup the second part of the animation. 
		 * It is often the case that the front condition fails, maybe because of normalization issues. 
		 * Nonetheless once the position vector reaches its target, the front vector basically does too. */
		if (currPos == midPos || currFront == midFront)
		{
			m_cam3D.setPosition(midPos);
			m_cam3D.setFront(midFront);
			m_animMidTime = glfwGetTime();
			m_animSecondPart = true;
		}
	}
	// #################################################################################################################
	// ##################################    Animation last part: mid to final point    ################################
	// #################################################################################################################
	else
	{
		/* Time elapsed since second part of animation started. */
		float elapsed = glfwGetTime() - m_animMidTime;
		/* Interpolation variable.
		 * It is 0 at the beginning of the second part of the animation 
		 * and 1 when elapsed equals the duration of the second part of the animation. */
		float t = glm::clamp(elapsed / m_animSecondPartDuration, 0.0f, 1.0f);

		/* Interpolation of position vector. */
		glm::vec3 currPos = glm::mix(midPos, endPos, t);

		/* Set camera position to current position to animate the scene. */
		m_cam3D.setPosition(currPos);

		/* Once current position reached its target, end the animation 
		 * and block 3D inputs to enable 2D inputs for the game. */
		if (currPos == endPos) 
		{
			setInput2D(true);
			m_animationIsOn = false;
		}

	}
	return m_animationIsOn;
}


void Scene::drawScene() const
{
	/* Gets an alias for the shader from the res manager.*/
	Shader& basic = ResourceManager::getShader("basic");

	/* Aim assistant drawcall. */
	//if (m_aimIsOn) drawAim(ResourceManager::getShader("basic2D"));

	/* Activates the shader. */
	basic.use();

	/* Room draw call.*/
	basic.setMatrix4fv("model", 1, GL_FALSE, m_room.getModelMat());
	m_room.draw(basic);

	/* Arcade draw call. */
	basic.setMatrix4fv("model", 1, GL_FALSE, m_arcade.getModel().getModelMat());	// TODO: getmodelmat for arcade class
	m_arcade.draw(basic);


}

void Scene::drawAim() const
{
	if (m_aimIsOn) drawAim(ResourceManager::getShader("basic2D"));
}

void Scene::cam3DinputHandler(GLFWwindow* window, const ActionMap& actionMap3D)
{
	//if (m_animationIsOn) m_cursorCentered = false;	// added to cameraAnimation.
	//if (!m_useCam3D || m_animationIsOn) return;		// added to input3DHandler.

	/* ------- Keyboard input: WASD movements. ------- */
	if (actionMap3D.ongoing(Action::MoveForward))	m_cam3D.moveForward(m_cam3D.getKeyboardSpeed());
	if (actionMap3D.ongoing(Action::MoveLeft))		m_cam3D.moveLeft(m_cam3D.getKeyboardSpeed());
	if (actionMap3D.ongoing(Action::MoveBackward))	m_cam3D.moveBackward(m_cam3D.getKeyboardSpeed());
	if (actionMap3D.ongoing(Action::MoveRight))		m_cam3D.moveRight(m_cam3D.getKeyboardSpeed());

	/* ------- Mouse input: rotations. ------- */

	/* Retrieving window center coordinates. */
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	int centerX = std::floor(width * 0.5);
	int centerY = std::floor(height * 0.5);


	/* Setting cursor at the window center. */
	if (!m_cursorCentered)
	{
		glfwSetCursorPos(window, centerX, centerY);
		m_lastX = centerX;
		m_lastY = centerY;
		m_cursorCentered = true;
		return; // skips first frame rotations
	}
	/* Retrieving cursor coords. */
	double posX, posY;
	glfwGetCursorPos(window, &posX, &posY);

	/* Offset since last frame. */
	double offX = posX - m_lastX;
	double offY = m_lastY - posY;

	/* Updates cursor coords. */
	m_lastX = posX;
	m_lastY = posY;

	/* Rotation. */
	if (offX != 0.0 || offY != 0.0) m_cam3D.processMouseInputs(static_cast<float>(offX), static_cast<float>(offY));
}

void Scene::drawAim(Shader& shader) const
{
	glm::mat4 aimProj = glm::ortho(0.0f, float(m_width), 0.0f, float(m_height));
	glm::mat4 aimModel = glm::translate(glm::mat4(1.0f), glm::vec3(m_width * 0.5f, m_height * 0.5f, 0.0f));
	shader.use();
	shader.setMatrix4fv("projection", 1, GL_FALSE, aimProj);
	shader.setMatrix4fv("model", 1, GL_FALSE, aimModel);
	glBindVertexArray(m_aimVAO);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

void Scene::initScene()
{
	/* Shift the WCS position by a position vector and set the model matrix of the arcade model. */
	glm::mat4 arcadeModelMat = glm::translate(glm::mat4(1.0f), sceneData::arcadeModelPositionShift);	
	m_arcade.getModel().setModelMat(arcadeModelMat);

	/* Set the model matrix of the room model to the 4x4 identity matrix. */
	m_room.setModelMat(glm::mat4(1.0f));
}

void Scene::initAim()
{
	float aimLength = 10.0f;
	float aimVerts[] =
	{
		// pos              // color
		// horizontal line
		-aimLength, 0.0f,   1.0f, 0.0f, 0.0f,
		 aimLength, 0.0f,   1.0f, 0.0f, 0.0f,

		 // vertical line
		 0.0f, -aimLength,   1.0f, 0.0f, 0.0f,
		 0.0f,  aimLength,   1.0f, 0.0f, 0.0f
	};

	unsigned int aimVBO;
	glGenVertexArrays(1, &m_aimVAO);
	glGenBuffers(1, &aimVBO);

	glBindVertexArray(m_aimVAO);
	glBindBuffer(GL_ARRAY_BUFFER, aimVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(aimVerts), aimVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}



// #################################################################################################################
// ##############################################   DESCARDED METHODS   ############################################
// #################################################################################################################


/* TODO: provare a sostituire distance con lenght2 per evitare i calcoli di radici quadrate
 * confronto: distance(v,w) > eps ------> lenght2(v,w) > eps*eps	dovrebbe essere più efficiente
 * in questo caso non cambierebbe quasi nulla, ma in casi dove per ogni chiamata di funzione ci sono
 * migliaia/milioni di glm::distance allora può fare molta differenza: distance calcola sqrt mentre lenght2 no
 * e sqrt è computazionalmente intensivo per la CPU.
 *
 * Old camera animation frame-based. Replaced with time-based smoother animation, less manual.
 */
//bool Scene::cameraAnimationOLD()
//{
//	/* cursorCentered is set to false so the cursor gets centered again
//	 * at the end of the animation, because cursor position can still change during animation. */
//	m_cursorCentered = false;
//
//	/* Disable fixed height. */
//	m_cam3D.setGrounded(false);
//
//	/* Display center is a point so it needs the offset of the model position in WCS;*/
//	glm::vec3 displayCenter = m_arcade.getModel().getMesh("display").getCenter() + m_arcade.getModel().getWCSPosition();
//	/* Display normal establishes a direction that does not need a translation. */
//	glm::vec3 displayNormal = m_arcade.getModel().getMesh("display").getGlobalNormal();
//
//	/* Intermediate point position. Let it be dC + 3*dN. */
//	glm::vec3 midPos = displayCenter + 3.0f * displayNormal;
//	/* The front vector should point at the display center, so it is the opposite of the display normal.*/
//	glm::vec3 midFront = -displayNormal;
//	/* Final point position. Let it be dC + 0.67*dN.
//	 * At this point the front is already on the same direction (opposite orientation).*/
//	glm::vec3 endPos = displayCenter + 0.67f * displayNormal;
//
//	/* Retrieve current position and front vectors. */
//	glm::vec3 currPos = m_cam3D.getPosition();
//	glm::vec3 currFront = m_cam3D.getFront();
//
//	/* First time entering in the function, setting starting position and front vectors (Scene struct members). */
//	if (!m_animationIsOn)
//	{
//		m_startPos = currPos;
//		m_startFront = currFront;
//		m_animationIsOn = true;
//		m_aimIsOn = false;
//	}
//
//	// #################################################################################################################
//	// ##################################   Animation first part: current to mid point   ###############################
//	// #################################################################################################################
//
//	/* ----- Camera animation from starting position and front to the intermediate point position and front. ------ */
//	if (!m_animLastPart)
//	{
//		/* Step < 2epsilon to ensure convergence. (epsilon >0.5step) */
//		const float step = sceneData::arcadeAnimationSpeed;
//		float eps = step * 0.51;
//
//		/* Initial vector distances for position and front.*/
//		float initPosGap = glm::distance(m_startPos, midPos);
//		float initFrontGap = glm::distance(m_startFront, midFront);
//
//		/* Direction vectors for the interpolation. */
//		glm::vec3 stepPosDir = glm::normalize(midPos - m_startPos);
//		glm::vec3 stepFrontDir = glm::normalize(midFront - m_startFront);
//
//		/* Number of steps for both interpolation must be the same.
//		 * Number of steps = initial vector distance / step size.
//		 * If one calls Lp: initPosGap, Lf:initFrontGap, STp: stepPos, STf: stepFront, then
//		 * Lp/STp = Lf/STf ----> if one parametrizes STp = step, then
//		 * if Lp is not zero --> STf = Lf/Lp * STp
//		 * else STf = step; */
//		float stepPos = step;
//		float stepFront;
//		if (initPosGap >= eps) stepFront = initFrontGap / initPosGap * stepPos;
//		else stepFront = step;
//
//		/* Current position gap. */
//		float currPosGap = glm::distance(currPos, midPos);
//		/* Position gap after the next step. */
//		float stepPosGap = glm::distance(currPos + stepPos * stepPosDir, midPos);
//		/* Current front gap. */
//		float currFrontGap = glm::distance(currFront, midFront);
//		/* Front gap after the next step. */
//		float stepFrontGap = glm::distance(currFront + stepFront * stepFrontDir, midFront);
//
//		/* If one of the gaps is still > epsilon the function returns animationIsOn = true, else returns false */
//		if (currPosGap >= eps || currFrontGap >= eps)
//		{
//			/* This is valid for both position and front vectors.
//			 * If the current gap is greater than epsilon, check if after a step it's still greater than epsilon.
//			 * If it is, make the step, else make the current vector equal to the final one.
//			 * Then update the camera vector member value. */
//
//			if (currPosGap >= eps)
//			{
//				if (stepPosGap >= eps) currPos += stepPos * stepPosDir;
//				else currPos = midPos;
//				m_cam3D.setPosition(currPos);
//
//			}
//			if (currFrontGap >= eps)
//			{
//				if (stepFrontGap >= eps) currFront += stepFront * stepFrontDir;
//				else currFront = midFront;
//				m_cam3D.setFront(currFront);
//			}
//
//			return m_animationIsOn;
//		}
//		else
//		{
//			//m_aimIsOn = true;
//			//m_animationIsOn = false;
//			//setSceneInput(false);
//			m_animLastPart = true;
//			return m_animationIsOn;
//
//		}
//	}
//	// #################################################################################################################
//	// ##################################    Animation last part: mid to final point    ################################
//	// #################################################################################################################
//	else
//	{
//		/* Step < 2epsilon to ensure convergence. (epsilon >0.5step) */
//		const float step = sceneData::arcadeAnimationSpeed;
//		float eps = step * 0.51;
//
//		/* Direction of the steps (it is the front vector of the intermediate phase). */
//		glm::vec3 stepPosDir = midFront;
//
//		/* Current position gap. */
//		float currPosGap = glm::distance(currPos, endPos);
//		/* Position gap after the next step. */
//		float stepPosGap = glm::distance(currPos + step * stepPosDir, endPos);
//
//		/* If the current gap is greater than epsilon, check if after a step it's still greater than epsilon.
//		 * If it is, make the step, else make the current vector equal to the final one.
//		 * Then update the camera vector member value. */
//		if (currPosGap >= eps)
//		{
//			if (stepPosGap >= eps) currPos += step * stepPosDir;
//			else currPos = endPos;
//			m_cam3D.setPosition(currPos);
//			return m_animationIsOn;
//		}
//		else	// animation is over.
//		{
//			m_animLastPart = false;
//			m_animationIsOn = false;
//			setSceneInput(false);		// disable 3D scene inputs -> indirectly activates 2D game inputs
//			return m_animationIsOn;
//		}
//	}
//
//}