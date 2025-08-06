#include "camera.h"

#include<glm/gtx/transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const float& fovy, const float& aspect, const float& near, const float& far):
	m_position(position),
	m_target(target),
	//m_VUP(glm::vec3(0.0f, 1.0f, 0.0f)),		// m_VUP is y-axis unit vector j (0,1,0). [OpenGL convention]
	m_up(glm::vec3(0.0f)),						// Initialize m_up at vector null.
	m_fovy(fovy),								// In degrees.
	m_aspect(aspect),	
	m_near(near),
	m_far(far)
{
	// Computing up.
	// w = normalize(position - target);
	glm::vec3 w = glm::normalize(position - target);
	// u = normalize(cross(VUP,w)); by openGL conventions the VUP is j, the y-axis unit vector (0,1,0);
	glm::vec3 u = glm::normalize(glm::cross(m_VUP, w));
	// up = cross(w,u); it is already normalized. */
	m_up = glm::cross(w, u);
}

glm::mat4 Camera::getPerspectiveProjMatrix() const
{
	return glm::perspective(glm::radians(m_fovy), m_aspect, m_near, m_far);
}

glm::mat4 Camera::getOrthoProj(const int& width, const int& height) const
{
	return glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_position, m_target, m_up);
}


