#include "camera.h"
#include "../core/validation.h"
#include <algorithm>
#include <cmath>
#include "../app/scene3D/scene_data.h"
#include<glm/gtx/transform.hpp>
//#include<iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera() :										// Orientation from the original scene defaults.
	m_altitude(sceneData::cameraAltitude),
	m_position(sceneData::cameraStartingPosition),
	m_front(glm::normalize(glm::vec3(-m_position.x, 0.0f, 0.0f))),	// Initialized at -position.x (viewing straight front)
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),			// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)), 							// If direction instead of front: cross(dir, right)
	m_yaw(std::atan2(m_front.x, m_front.z)),						// atan2 considers all 4 quadrants and returns radians
	m_pitch(std::asin(m_front.y)),									// asin returns radians
	m_fovy(sceneData::cameraFovy),									// In degrees.
	m_aspect(sceneData::cameraAspect),								// TODO: default for now is same ratio as my display
	m_near(sceneData::cameraNearPlane),
	m_far(sceneData::cameraFarPlane),
	m_mouseSpeed(sceneData::cameraInitMouseSpeed),
	m_keyboardSpeed(sceneData::cameraInitKeyboardSpeed),
	m_grounded(sceneData::cameraInitialGrounded)

{ 
}


Camera::Camera(const glm::vec3& position, const float& fovy, const float& aspect, const float& near, const float& far) :
	m_altitude(position.y),
	m_position(position),
	m_front(glm::normalize(-position)),							// FRONT: opposite of classic direction (dir) vector of cameras
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),		// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)),							// If direction instead of front: cross(dir, right)
	m_yaw(std::atan2(m_front.x, m_front.z)),					// atan2 considers all 4 quadrants and returns radians
	m_pitch(std::asin(m_front.y)),								// asin returns radians
	m_fovy(fovy),												// In degrees.
	m_aspect(aspect),	
	m_near(near),
	m_far(far),
	m_mouseSpeed(sceneData::cameraInitMouseSpeed),
	m_keyboardSpeed(sceneData::cameraInitKeyboardSpeed),
	m_grounded(sceneData::cameraInitialGrounded)
{
    validation::require(validation::finite(position) && glm::length(position)>0
        && glm::length(glm::cross(m_front,m_VUP))>0 && validation::finite(fovy) && fovy>0 && fovy<180
        && validation::finite(aspect) && aspect>0 && validation::finite(near) && validation::finite(far)
        && near>0 && far>near,"Invalid camera position or projection parameters");
}

glm::mat4 Camera::getPerspectiveProjMatrix() const
{
	return glm::perspective(glm::radians(m_fovy), m_aspect, m_near, m_far);
}

glm::mat4 Camera::getOrthoProj(const int& width, const int& height) const
{
	/*TODO: still hardcoded, might review later. */
	validation::require(width>0 && height>0,"Viewport dimensions must be positive");
	return glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
	// Position + Front = Target DIRECTION
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processMouseInputs(const float& offX, const float& offY)
{
	rotateOnXAxis(offX);
	rotateOnYAxis(offY);
	//std::cout << "inside processMouseInputs\nm_yaw: " << m_yaw << "\tm_pitch:" << m_pitch << std::endl;
	if (m_pitch > glm::radians(89.0f)) m_pitch = glm::radians(89.0f);
	if (m_pitch < -glm::radians(89.0f)) m_pitch = -glm::radians(89.0f);

	updateCameraVectors();

}



void Camera::updateCameraVectors(bool frontNeedsUpdate)
{
	if(frontNeedsUpdate)
	{
		/* Recalculate front vector using yaw and pitch (already in radians).
		 * Normalize the final vector at the end. */
		m_front.x = glm::cos(m_pitch) * glm::sin(m_yaw);
		m_front.y = glm::sin(m_pitch);
		m_front.z = glm::cos(m_pitch) * glm::cos(m_yaw);
		m_front = glm::normalize(m_front);
	}
	else
	{
		/* Recalculate pitch and yaw using front vector. */
		m_pitch = std::asin(m_front.y);		// atan2 considers all 4 quadrants and returns radians
		m_yaw = std::atan2(m_front.x, m_front.z);
		if (m_pitch > glm::radians(89.0f)) m_pitch = glm::radians(89.0f);
		if (m_pitch < -glm::radians(89.0f)) m_pitch = -glm::radians(89.0f);
		//std::cout << "inside updateCamVecs\nm_yaw: " << m_yaw << "\tm_pitch:" << m_pitch << std::endl;

	}

	/* Computing camera up vector.
	 * Right = normalize(cross(front,VUP)); by openGL conventions the VUP is j, the y-axis unit vector (0,1,0).
	 * Updates up = cross(front,right); it is already normalized. */
	m_right = glm::normalize(glm::cross(m_front, m_VUP));
	m_up = glm::cross(m_right, m_front);

	// Saved altitude changes when position/grounding is explicitly changed.
	//std::cout << "m_front = (" << m_front.x << ", " << m_front.y << ", " << m_front.z << ")\n";
	
}





void Camera::setAspect(float value) {
    validation::require(validation::finite(value) && value>0,"Camera aspect must be positive"); m_aspect=value;
}
void Camera::setGrounded(float value) {
    validation::require(validation::finite(value),"Grounded flag must be finite"); m_grounded=value!=0; m_altitude=m_position.y;
}
void Camera::setPosition(const glm::vec3& value) {
    validation::require(validation::finite(value),"Camera position must be finite"); m_position=value; m_altitude=value.y;
}
void Camera::setFront(const glm::vec3& value) {
    validation::require(validation::finite(value) && glm::length(value)>0,"Camera front must be finite and nonzero");
    m_front=glm::normalize(value); updateCameraVectors(false); updateCameraVectors(true);
}
void Camera::setKeyboardSpeed(float value) {
    validation::require(validation::finite(value) && value>=0,"Camera speed must be nonnegative"); m_keyboardSpeed=value;
}
void Camera::setMouseSpeed(float value) {
    validation::require(validation::finite(value) && value>=0,"Mouse sensitivity must be nonnegative"); m_mouseSpeed=value;
}
void Camera::moveForward(float dt) { validation::require(validation::finite(dt),"Camera time must be finite"); m_position+=m_keyboardSpeed*m_front*dt; if(m_grounded)m_position.y=m_altitude; }
void Camera::moveBackward(float dt) { moveForward(-dt); }
void Camera::moveRight(float dt) { validation::require(validation::finite(dt),"Camera time must be finite"); m_position+=m_keyboardSpeed*m_right*dt; }
void Camera::moveLeft(float dt) { moveRight(-dt); }
void Camera::moveUp(float dt) { validation::require(validation::finite(dt),"Camera time must be finite"); m_position+=m_keyboardSpeed*m_up*dt; if(m_grounded)m_altitude=m_position.y; }
void Camera::moveDown(float dt) { moveUp(-dt); }
void Camera::rotateOnXAxis(float dx) { validation::require(validation::finite(dx),"Mouse offset must be finite"); m_yaw-=dx*m_mouseSpeed; updateCameraVectors(); }
void Camera::rotateOnYAxis(float dy) { validation::require(validation::finite(dy),"Mouse offset must be finite"); m_pitch=std::clamp(m_pitch+dy*m_mouseSpeed,-glm::radians(89.0f),glm::radians(89.0f)); updateCameraVectors(); }
