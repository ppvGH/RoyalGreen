#include "scene.h"
#include "../path.h"
#include "scene_data.h"

#include "../../core/resource_manager.h"
#include <glm/gtx/transform.hpp>

#include <iostream> //for debug
#include "../../core/utilities.h"

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
	m_gameOn(false),
	m_cam3D(),
	m_cursorCentered(false),
	m_lastX((double)width * 0.5),
	m_lastY((double)height * 0.5),
	m_room(Path::pathRoom, &ResourceManager::getTexture("floorTile")),
	m_arcade(Path::pathModel),
	m_pool(Path::pathPool, &ResourceManager::getShader(sceneData::blinnPhongShaderName)),
	m_mainLamp(Path::pathMainLamp, &ResourceManager::getShader(sceneData::blinnPhongShaderName)),
	m_tableLamp(Path::pathLamp, &ResourceManager::getShader(sceneData::blinnPhongShaderName)),
	m_tmp(Path::pathModel, &ResourceManager::getShader(sceneData::blinnPhongShaderName)),
	m_mainPointLightPos(glm::vec3(0.0f)),		// position is set in the constructor body after the initScene() method
	m_tablePointLightPos(glm::vec3(0.0f)),		// position is set in the constructor body after the initScene() method
	// m_spotLightPos(glm::vec3(0.0f)),
	// m_spotLightTarget(glm::vec3(0.0f)),
	// m_spotLightSpace(glm::mat4(0.0f)),
	m_animInIsOn(false),
	m_animOutIsOn(false),
	m_animSecondPart(false),
	m_startPos(glm::vec3(0.0f)),
	m_startFront(glm::vec3(0.0f)),
	m_animStartTime(0.0f),
	m_animMidTime(0.0f),
	m_animFirstPartDuration(sceneData::inAnimationFirstPartDuration),
	m_animSecondPartDuration(sceneData::inAnimationSecondPartDuration),
	m_aimVAO(0),
	m_aimIsOn(true),
	m_mainDepthCubeFBO(sceneData::shadowWidth, sceneData::shadowHeight, sceneData::FBOtypeCubeDepth),
	m_tableDepthCubeFBO(sceneData::shadowWidth, sceneData::shadowHeight, sceneData::FBOtypeCubeDepth)
	//m_depthSpotFBO(sceneData::shadowWidth, sceneData::shadowHeight, sceneData::FBOtypeDepth)
{
	/* Set aspect = width/height for camera 3D. */
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	m_cam3D.setAspect(aspect);

	/* Initialize scene models and their model matrices. */
	initScene();

	/* Save positions for light sources for the shader passes. */
	m_mainPointLightPos = m_mainLamp.getWCSPosition() + m_mainLamp.getMesh("lamp_light").getCenter();
	m_tablePointLightPos = m_tableLamp.getWCSPosition() + m_tableLamp.getMesh("lamp_plastic").getCenter() + 
		glm::vec3(0.0f, sceneData::tableLampHeightFix, 0.0f);

	//m_spotLightPos = m_tableLamp.getWCSPosition() + m_tableLamp.getMesh("lamp_plastic").getCenter();
	//m_spotLightPos = m_pool.getWCSPosition() + glm::vec3(0.0f, 3.0f, 0.0f);
	//m_spotLightTarget = m_pool.getWCSPosition() + glm::vec3(0.0f, +0.8f, 0.0f);

	/* Initialize aim assistant (sight). */
	initAim();

}

void Scene::input3DHandler(GLFWwindow* window, const ActionMap& actionMap3D)
{
	/* Disable 3D cam inputs when animation is ongoing. */
	if (!isAnyAnimationOn()) cam3DinputHandler(window, actionMap3D);	//removed m_usecam3D from conditions if(m_useCam3D && ..)

	if (actionMap3D.justStarted(Action::StartAnimation))	// just during testing (bind to key K)
	{
		cameraInAnimation();
	}

	if (actionMap3D.justStarted(Action::SwitchScreen)) switchArcadeScreen();

	if (actionMap3D.justStarted(Action::SelectObject))
		if (picking())
		{
			m_cursorCentered = false;
			openArcadeMenu();	
		}			
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

bool Scene::picking() const
{
	bool hit = false;

	const Model& testModel = m_arcade.getModel();
	glm::mat4 inverseModel = glm::inverse(testModel.getModelMat());
	glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(m_cam3D.getPosition(),1.0f));
	glm::vec3 localDirect = glm::vec3(inverseModel * glm::vec4(m_cam3D.getFront(), 0.0f));			

	Ray localRay(localOrigin, localDirect);

	float t;
	hit = testModel.intersectRayTriangle(localRay,t);

	return hit;
}

void Scene::update()
{
	if (m_animInIsOn) cameraInAnimation();
	if (m_animOutIsOn) cameraOutAnimation();
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
	if (!m_animInIsOn)
	{
		if (!isDisplayOn()) switchArcadeScreen();	// turn on display if off

		m_startPos = m_cam3D.getPosition();
		m_startFront = m_cam3D.getFront();

		m_aimIsOn = false;

		m_animStartTime = glfwGetTime();

		m_animSecondPart = false;

		setInput3D(false);

		m_animInIsOn = true;
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
			m_animInIsOn = false;
			m_gameOn = true;
		}

	}
	return m_animInIsOn;
}

bool Scene::cameraOutAnimation()
{
	/* cursorCentered is set to false so the cursor gets centered again
	 * at the end of the animation, because cursor position can still change during animation. */
	m_cursorCentered = false;

	/* Display Center is a point so it needs the offset of the model position in WCS. */
	glm::vec3 dC = m_arcade.getModel().getMesh("display").getCenter() + m_arcade.getModel().getWCSPosition();
	/* Display Normal establishes a direction that does not need a translation. */
	glm::vec3 dN = m_arcade.getModel().getMesh("display").getGlobalNormal();

	/* Final camera position. */
	glm::vec3 endPos = dC + 5.0f * dN; endPos.y = sceneData::cameraAltitude;
	//glm::vec3 endPos = glm::vec3((dC + 3.0f * dN).x, sceneData::cameraAltitude, (dC));

	/* First time entering in the function. */
	if (!m_animOutIsOn)
	{
		m_startPos = m_cam3D.getPosition();

		m_aimIsOn = true;

		m_animStartTime = glfwGetTime();

		setInput3D(true);
		setInput2D(false);

		m_animOutIsOn = true;
		m_gameOn = false;
	}

	/* Time elapsed since animation started. */
	float elapsed = glfwGetTime() - m_animStartTime;
	/* Interpolation variable.
	* It is 0 at the beginning of the animation
	* and 1 when elapsed equals the duration of the animation.
	* Duration of the animation is the same of the second part of the in animation. */
	float t = glm::clamp(elapsed / m_animSecondPartDuration, 0.0f, 1.0f);

	/* Interpolation of position vector. */
	glm::vec3 currPos = glm::mix(m_startPos, endPos, t);

	/* Set camera position to current position to animate the scene. */
	m_cam3D.setPosition(currPos);

	/* Once current position reached its target, end the animation
	 * and block 3D inputs to enable 2D inputs for the game. */
	if (currPos == endPos)
	{
		m_cam3D.setGrounded(true);
		m_animOutIsOn = false;
	}
	
	return m_animOutIsOn;
}

void Scene::drawScene()
{
	/* TODO: separate uniforms setup that dont change during scene rendering and setup them just once in the main before render loop. */

	/* Viewport and Buffers setup. */
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* BlinnPhong shader setup. */
	Shader& phong = ResourceManager::getShader(sceneData::blinnPhongShaderName).use();
	// Camera setup
	phong.setMatrix4fv("view", 1, GL_FALSE, m_cam3D.getViewMatrix());
	phong.setMatrix4fv("proj", 1, GL_FALSE, m_cam3D.getPerspectiveProjMatrix());
	phong.setVector3f("viewPos", m_cam3D.getPosition());
	// MAIN Point light setup
	phong.setVector3f("mainPLPos", m_mainPointLightPos);
	phong.setInt("mainPLDepthMap", 1);
	phong.setFloat("mainPLFarPlane", sceneData::mainPointLightFarPlane);
	/* MAIN Cubemap texture setup. */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_mainDepthCubeFBO.getTex().getID());

	// TABLE Point light setup
	phong.setVector3f("tablePLPos", m_tablePointLightPos);
	phong.setInt("tablePLDepthMap", 2);
	phong.setFloat("tablePLFarPlane", sceneData::tablePointLightFarPlane);
	phong.setFloat("tableLightHeightFix", sceneData::tableLampHeightFix);
	phong.setFloat("tableCosInner", sceneData::tableCosInnerAngle);
	phong.setFloat("tableCosOuter", sceneData::tableCosOuterAngle);
	phong.setFloat("invCosDelta", sceneData::tableInvCosDelta);

	/* MAIN Cubemap texture setup. */
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_tableDepthCubeFBO.getTex().getID());

	// Spot light setup
	/*phong.setMatrix4fv("lightSpace", 1, GL_FALSE, m_spotLightSpace);
	phong.setVector3f("spotLightTar", m_spotLightTarget);
	phong.setVector3f("spotLightPos", m_spotLightPos);
	phong.setInt("spotLightDepthMap", 2);*/

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, m_depthSpotFBO.getTex().getID());
	
	/* CRT shader setup. */
	Shader& CRT = ResourceManager::getShader(sceneData::CRTShaderName).use();
	CRT.setMatrix4fv("view", 1, GL_FALSE, m_cam3D.getViewMatrix());
	CRT.setMatrix4fv("proj", 1, GL_FALSE, m_cam3D.getPerspectiveProjMatrix());

	/* Draw calls. */
	drawModels(sceneData::blinnPhongShaderName);
		
	/* Aim draw call. */
	if (m_aimIsOn) drawAim(ResourceManager::getShader(sceneData::aimShaderName));

}

void Scene::pointLightShadowMap(glm::vec3 lightPos, Framebuffer& fbo, float nearPlane, float farPlane)
{
	/* Lamp WCS position is the light position.*/
	//glm::vec3 lightPos = m_mainLamp.getMesh("lamp_light").getCenter() + m_mainLamp.getWCSPosition();
	
	/* Light projection matrix. */
	glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

	/* Data for light view matrices (one for each cube face). */
	glm::vec3 axes[6] =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),	// +X
		glm::vec3(-1.0f, 0.0f, 0.0f),	// -X
		glm::vec3(0.0f, 1.0f, 0.0f),	// +Y
		glm::vec3(0.0f, -1.0f, 0.0f),	// -Y
		glm::vec3(0.0f, 0.0f, 1.0f),	// +Z
		glm::vec3(0.0f, 0.0f, -1.0f)	// -Z
	};
	glm::vec3 VUPs[6] =
	{
		glm::vec3(0.0f, -1.0f, 0.0f),	// for +X
		glm::vec3(0.0f, -1.0f, 0.0f),	// for -X
		glm::vec3(0.0f, 0.0f, 1.0f),	// for +Y
		glm::vec3(0.0f, 0.0f, -1.0f),	// for -Y
		glm::vec3(0.0f, -1.0f, 0.0f),	// for +Z
		glm::vec3(0.0f, -1.0f, 0.0f),	// for -Z
	};

	/* Light view matrices. */
	std::vector<glm::mat4> lightPVs;
	for (int i = 0; i < 6; i++) 
	{
		lightPVs.push_back(lightProj * glm::lookAt(lightPos, lightPos + axes[i], VUPs[i]));
	}


	/* Depth pass. */
	/* FBO and viewport setup. */
	glViewport(0, 0, sceneData::shadowWidth, sceneData::shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.getID());
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Depth shader setup. */
	Shader& depthShader = ResourceManager::getShader(sceneData::pointLightDepthShaderName).use();
	for (int i = 0; i < 6; i++)
	{
		std::string unif = "shadowMats[" + std::to_string(i) + "]";
		depthShader.setMatrix4fv(unif, 1, GL_FALSE, lightPVs[i]);
	}
	depthShader.setFloat("invFarPlane", 1/farPlane);
	depthShader.setVector3f("lightPos", lightPos);

	/* Render depth pass. */
	drawModels(sceneData::pointLightDepthShaderName);

	/* Unbind FBO. */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);

	
}

void Scene::depthPass()
{
	/* Main light depth pass. */
	pointLightShadowMap(m_mainPointLightPos, m_mainDepthCubeFBO, sceneData::mainPointLightNearPlane, sceneData::mainPointLightFarPlane);
	/* Table light depth pass. */
	pointLightShadowMap(m_tablePointLightPos, m_tableDepthCubeFBO, sceneData::tablePointLightNearPlane, sceneData::tablePointLightFarPlane);

}

void Scene::spotLightShadowMap(glm::vec3 lightPos, glm::vec3 lightTarget, glm::mat4& spotLightSpace, Framebuffer& fbo, float fovy, float aspect, float nearPlane, float farPlane)
{
	
	/* Light projection matrix. */
	glm::mat4 lightProj = glm::perspective(glm::radians(fovy), aspect, nearPlane, farPlane);

	glm::vec3 VUP = glm::vec3(0.0f, 0.0f, -1.0f);
	/* Light view matrix. */
	glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, VUP);

	/* Light space matrix. */
	glm::mat4 lightSpaceMatrix = lightProj * lightView;
	spotLightSpace = lightSpaceMatrix;
	//printMat4(lightSpaceMatrix);

	/* Depth pass.*/
	/* FBO and viewport setup. */
	glViewport(0, 0, sceneData::shadowWidth, sceneData::shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.getID());
	glClear(GL_DEPTH_BUFFER_BIT);

	/* Depth shader setup. */
	Shader& shad = ResourceManager::getShader(sceneData::spotLightDepthShaderName).use();
	shad.setMatrix4fv("lightSpace", 1, GL_FALSE, lightSpaceMatrix);

	/* Render depth pass. */
	drawModels(sceneData::spotLightDepthShaderName);

	/* Unbind FBO. */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::drawModels(const std::string& shaderName)
{
	/* Room draw call. */
	m_room.setShader(shaderName);
	m_room.setWCSPosition();
	m_room.draw();

	/* Arcade draw call. */
	m_arcade.setShader(shaderName);
	m_arcade.setWCSPosition();
	m_arcade.draw();

	/* Pool draw call. */
	m_pool.setShader(shaderName);
	m_pool.setWCSPosition();
	m_pool.draw();

	/* Main lamp draw call. */
	m_mainLamp.setShader(shaderName);
	m_mainLamp.setWCSPosition();
	m_mainLamp.draw();

	/* Table lamp draw call. */
	m_tableLamp.setShader(shaderName);
	m_tableLamp.setWCSPosition();
	m_tableLamp.draw();

}

void Scene::initScene()
{
	/* Shift the WCS position by the arcademodel position vector and set the model matrix. */
	glm::mat4 arcadeModelMat = glm::translate(glm::mat4(1.0f), sceneData::arcadeModelPositionShift);	
	m_arcade.getModel().setModelMat(arcadeModelMat);

	/* Set the model matrix of the room model to the 4x4 identity matrix. */
	m_room.getModel().setModelMat(glm::mat4(1.0f));

	/* Shift WCS position by the pool position vector and set the model matrix. */
	glm::mat4 poolModelMat = glm::translate(glm::mat4(1.0f), sceneData::poolModelPositionShift);
	m_pool.setModelMat(poolModelMat);

	/* Shift WCS position by the main lamp position vector and set the model matrix. */
	glm::mat4 mainLampModelMat = glm::translate(glm::mat4(1.0f), sceneData::mainLampModelPositionShift);	
	m_mainLamp.setModelMat(mainLampModelMat);

	/* Shift WCS position by the lampmodel position vector and set the model matrix. */
	glm::mat4 tableLampModelMat = glm::translate(glm::mat4(1.0), sceneData::tableLampModelPositionShift);	// up the lamp
	tableLampModelMat = glm::translate(tableLampModelMat, m_pool.getWCSPosition());			// put it above the table
	m_tableLamp.setModelMat(tableLampModelMat);

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
