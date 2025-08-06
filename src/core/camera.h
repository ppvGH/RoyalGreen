#pragma once

#include <glm/glm.hpp>
#include <string>




class Camera
{
public:

	/* No default constructor allowed. */
	Camera() = delete;

	/* Parametric constructor. 
	 + fovy: expressed in degrees, NOT radians; */
	Camera(const glm::vec3& position, const glm::vec3& target, const float& fovy, const float& aspect, const float& near, const float& far);




	// Getters

	/* Returns prospective projection matrix. */
	glm::mat4 getPerspectiveProjMatrix() const;

	/* TODO: Returns orthogonal projection matrix.*/
	glm::mat4 getOrthoProj(const int& width, const int& height) const;

	/* Returns view matrix. */
	glm::mat4 getViewMatrix() const;




private:
	/* The eye of the camera. */
	glm::vec3 m_position;

	/* Where the camera looks at. */
	glm::vec3 m_target;
	
	/* By OpenGL convention, VUP corresponds to the y-axis unit vector (0,1,0). */
	const glm::vec3 m_VUP = glm::vec3(0.0f, 1.0f, 0.0f);

	/* Defines the inclination and the orientation of the camera. */
	glm::vec3 m_up;
	
	/* Angle from the bottom to the top of the view frustum.
	 * Expressed in degrees, must be in the interval [0, 180]. */
	float m_fovy;

	/* Ratio between width and height of the viewplane (near plane of the frustum). */
	float m_aspect;

	/* Near distance plane of the frustum. */
	float m_near;

	/* Far distance plane of the frustum. */
	float m_far;
};