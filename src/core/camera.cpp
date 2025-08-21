#include "camera.h"

#include<glm/gtx/transform.hpp>


Camera::Camera():
	m_position(glm::vec3(5.0f)),
	m_front(glm::normalize(-m_position)),						// Initialized at -position (target at origin)
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),		// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)), 						// If direction instead of front: cross(dir, right)
	m_pitch(std::asinf(m_front.y)),								// atan2 considers all 4 quadrants and returns radians 
	m_yaw(std::atan2f(m_front.x, m_front.z)),					// asin returns radians
	m_fovy(45.0f),												// In degrees.
	m_aspect(1.0f),
	m_near(0.1f),
	m_far(200.0f)
{ 
}


Camera::Camera(const glm::vec3& position, const float& fovy, const float& aspect, const float& near, const float& far) :
	m_position(position),
	m_front(glm::normalize(-position)),							// FRONT: opposite of classic direction (dir) vector of cameras
	m_right(glm::normalize(glm::cross(m_front, m_VUP))),		// If direction instead of front: cross(VUP, dir)
	m_up(glm::cross(m_right, m_front)),							// If direction instead of front: cross(dir, right)
	m_pitch(std::atan2f(m_front.x, m_front.z)),					// atan2 considers all 4 quadrants and returns radians 
	m_yaw(std::asinf(m_front.y)),								// asin returns radians
	m_fovy(fovy),												// In degrees.
	m_aspect(aspect),	
	m_near(near),
	m_far(far)
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
	// Position + Front = Target
	return glm::lookAt(m_position, m_position + m_front, m_up);
}



void Camera::updateCameraVectors()
{
	/* Recalculate front vector using yaw and pitch (already in radians).
	 * Normalize the final vector at the end. */
	m_front.x = glm::cos(m_pitch) * glm::sin(m_yaw);
	m_front.y = glm::sin(m_pitch);
	m_front.z = glm::cos(m_pitch) * glm::cos(m_yaw);
	m_front = glm::normalize(m_front);

	/* Computing camera up vector.
	 * Right = normalize(cross(front,VUP)); by openGL conventions the VUP is j, the y-axis unit vector (0,1,0).
	 * Updates up = cross(front,right); it is already normalized. */
	m_right = glm::normalize(glm::cross(m_front, m_VUP));
	m_up = glm::cross(m_right, m_front);

}




