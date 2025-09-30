#include "camera.h"

#include<glm/gtx/transform.hpp>
//#include<iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const float ALTITUDE = 1.7f;

Camera::Camera() :										// INITLIST FOLLOWS MEMBER DECLARATION ORDER IN THE CLASS HEADER
	m_altitude(ALTITUDE),
	m_position(glm::vec3(4.0f, ALTITUDE, 0.0f)),
	m_front(glm::normalize(glm::vec3(-m_position.x, 0.0f, 0.0f))),	// Initialized at -position.x (viewing straight front)
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),			// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)), 							// If direction instead of front: cross(dir, right)
	m_yaw(std::atan2f(m_front.x, m_front.z)),						// atan2 considers all 4 quadrants and returns radians
	m_pitch(std::asinf(m_front.y)),									// asin returns radians 
	m_fovy(45.0f),													// In degrees.
	m_aspect((float) 16 / 9),										// TODO: default for now is same ratio as my display
	m_near(0.1f),
	m_far(200.0f),
	m_speed(0.001f),
	m_grounded(true)

{ 
}


Camera::Camera(const glm::vec3& position, const float& fovy, const float& aspect, const float& near, const float& far) :
	m_altitude(ALTITUDE),
	m_position(position),
	m_front(glm::normalize(-position)),							// FRONT: opposite of classic direction (dir) vector of cameras
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),		// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)),							// If direction instead of front: cross(dir, right)
	m_yaw(std::atan2f(m_front.x, m_front.z)),					// atan2 considers all 4 quadrants and returns radians 
	m_pitch(std::asinf(m_front.y)),								// asin returns radians
	m_fovy(fovy),												// In degrees.
	m_aspect(aspect),	
	m_near(near),
	m_far(far),
	m_speed(0.001f),
	m_grounded(true)
{
}

glm::mat4 Camera::getPerspectiveProjMatrix() const
{
	return glm::perspective(glm::radians(m_fovy), m_aspect, m_near, m_far);
}

glm::mat4 Camera::getOrthoProj(const int& width, const int& height) const
{
	/*TODO: still hardcoded, might review later. */
	return glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
	// Position + Front = Target DIRECTION
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processMouseInputs(const float& offX, const float& offY)
{
	m_yaw -= offX * m_speed;	//RotateOnXAxis(offX) method can be created TODO
	m_pitch += offY * m_speed;	//RotateOnYAxis(offY) method can be created TODO
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
		m_pitch = std::asinf(m_front.y);		// atan2 considers all 4 quadrants and returns radians 
		m_yaw = std::atan2f(m_front.x, m_front.z);
		if (m_pitch > glm::radians(89.9f)) m_pitch = glm::radians(89.9f);
		if (m_pitch < -glm::radians(89.9f)) m_pitch = -glm::radians(89.9f);
		//std::cout << "inside updateCamVecs\nm_yaw: " << m_yaw << "\tm_pitch:" << m_pitch << std::endl;

	}

	/* Computing camera up vector.
	 * Right = normalize(cross(front,VUP)); by openGL conventions the VUP is j, the y-axis unit vector (0,1,0).
	 * Updates up = cross(front,right); it is already normalized. */
	m_right = glm::normalize(glm::cross(m_front, m_VUP));
	m_up = glm::cross(m_right, m_front);

	//std::cout << "m_front = (" << m_front.x << ", " << m_front.y << ", " << m_front.z << ")\n";
	
}




