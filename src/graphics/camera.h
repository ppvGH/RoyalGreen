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
	
	// Setters.
	/* Set ratio (m_aspect) to adjust ratio at runtime. */
	void setAspect(float aspect) { m_aspect = aspect; }

	void setGrounded(float grounded) { m_grounded = grounded; }

	void setPosition(const glm::vec3& position) { m_position = position; }

	void setFront(const glm::vec3& front) { m_front = front; updateCameraVectors(false); }


	// Getters.
	bool getGrounded() const { return m_grounded; }

	/* Returns prospective projection matrix. */
	glm::mat4 getPerspectiveProjMatrix() const;

	/* Returns orthogonal projection matrix.
	 + args: viewport width and height. They get static casted into floats inside the function. */
	glm::mat4 getOrthoProj(const int& width, const int& height) const;

	/* Returns view matrix. */
	glm::mat4 getViewMatrix() const;

	/* Returns camera position.*/
	glm::vec3 getPosition() const { return m_position; }

	/* Returns camera front. */
	glm::vec3 getFront() const { return m_front; }

	/* Returns target = position + front. */
	glm::vec3 getTarget() const { return m_position + m_front; }

	float getYaw() const { return m_yaw; }
	float getPitch() const { return m_pitch; }


	/* Returns mouse sensitivity. */
	float getMouseSpeed() const { return m_mouseSpeed; }
	/* Returns keyboard sensitivity. */
	float getKeyboardSpeed() const { return m_keyboardSpeed; }

	


	// Camera movements. Documentation TODO
	void moveForward(float dx) { m_position += m_keyboardSpeed * m_front * dx; if (m_grounded) m_position.y = m_altitude; }
	void moveBackward(float dx) { m_position -= m_keyboardSpeed * m_front * dx; if (m_grounded) m_position.y = m_altitude; }
	
	void moveRight(float dx) { m_position += m_keyboardSpeed * m_right * dx; }
	void moveLeft(float dx) { m_position -= m_keyboardSpeed * m_right * dx; }
	void moveUp(float dx) { m_position += m_keyboardSpeed * m_up * dx; }
	void moveDown(float dx) { m_position -= m_keyboardSpeed * m_up * dx; }

	void rotateOnXAxis(float offX) { m_yaw -= offX * m_mouseSpeed;}
	void rotateOnYAxis(float offY) { m_pitch += offY * m_mouseSpeed; }

	/* Utilized in mouse callback. */
	void processMouseInputs(const float& offX, const float& offY);

	/* Aim sign. */
	//void 


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
	
	/* Horizontal inclination. */
	float m_yaw;
	/* Vertical inclination. */
	float m_pitch;

	/* Angle from the bottom to the top of the view frustum.
	 * Expressed in degrees, must be in the interval [0, 180]. */
	float m_fovy;
	/* Ratio between width and height of the viewplane (near plane of the frustum). */
	float m_aspect;
	/* Near distance plane of the frustum. */
	float m_near;
	/* Far distance plane of the frustum. */
	float m_far;

	/* Sensitivity for mouse control inputs. */
	float m_mouseSpeed;
	/* Sensitivity for keyboard control inputs. */
	float m_keyboardSpeed;

	/* If true, position.y will stay fixed at the altitude height. */
	bool m_grounded;
	float m_altitude;

	/*TODO: documentation here*/
	/* If arg: frontNeedsUpdate is TRUE means that yaw and pitch has been changed and front needs to be updated (DEFAULT);
	 * If arg: frontNeedsUpdate is FALSE means that front has been changed and yaw and pitch need to be updated; */
	void updateCameraVectors(bool frontNeedsUpdate = true);


};