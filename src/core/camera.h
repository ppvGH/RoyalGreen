#pragma once

#include <glm/glm.hpp>
#include <string>




class Camera
{
public:

	/* TODO: documentation. */
	Camera();

	/* TODO: update documentation for Parametric constructor. 
	 + position: camera position in WCS (eye);
	 + target: where the camera is looking at in WCS;
	 + fovy: expressed in degrees, NOT radians;
	 + aspect: width/height ratio. better cast both to float if they are not;
	 + near: near distance plane;
	 + far: far distance plane; */
	Camera(const glm::vec3& position, const float& fovy, const float& aspect, const float& near, const float& far);

	// Getters.
	/* Returns prospective projection matrix. */
	glm::mat4 getPerspectiveProjMatrix() const;

	/* Returns orthogonal projection matrix.
	 + args: viewport width and height. They get static casted into floats inside the function. */
	glm::mat4 getOrthoProj(const int& width, const int& height) const;

	/* Returns view matrix. */
	glm::mat4 getViewMatrix() const;

	/* Returns camera position.*/
	inline glm::vec3 getPosition() const { return m_position; };


	// Camera movements. Documentation TODO
	inline void moveForward(const float& dx) { m_position += m_front * dx; }
	inline void moveBackward(const float& dx) { m_position -= m_front * dx; }

	inline void moveRight(const float& dx) { m_position += m_right * dx; }
	inline void moveLeft(const float& dx) { m_position -= m_right * dx; }
	inline void moveUp(const float& dx) { m_position += m_up * dx; }
	inline void moveDown(const float& dx) { m_position -= m_up * dx; }

	inline void turnRight(const float& psi) { m_yaw -= glm::radians(psi); updateCameraVectors();}
	inline void turnLeft(const float& psi) { m_yaw += glm::radians(psi); updateCameraVectors();}

	inline void turnUp(const float& theta) { m_pitch += glm::radians(theta); updateCameraVectors(); }
	inline void turnDown(const float& theta) { m_pitch -= glm::radians(theta); updateCameraVectors(); }


private:
	/* The eye of the camera. */
	glm::vec3 m_position;
	/* Direction of camera. Target - Position.*/
	glm::vec3 m_front;
	/* By OpenGL convention, VUP corresponds to the y-axis unit vector (0,1,0). */
	const glm::vec3 m_VUP = glm::vec3(0.0f, 1.0f, 0.0f);
	/* RHS of the camera. r = norm(cross(front, VUP)); */
	glm::vec3 m_right;
	/* Defines the inclination and the orientation of the camera.
	 * Gets computed initially in the constructor and must be calculated again in the setters. */
	glm::vec3 m_up;
	
	/* Vertical inclination. */
	float m_pitch;
	/* Horizontal inclination. */
	float m_yaw;

	/* Angle from the bottom to the top of the view frustum.
	 * Expressed in degrees, must be in the interval [0, 180]. */
	float m_fovy;
	/* Ratio between width and height of the viewplane (near plane of the frustum). */
	float m_aspect;
	/* Near distance plane of the frustum. */
	float m_near;
	/* Far distance plane of the frustum. */
	float m_far;



	/*TODO: documentation here*/
	void updateCameraVectors();


};