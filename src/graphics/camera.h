#pragma once

#include <glm/glm.hpp>
#include <string>




class Camera
{
public:

	/* Uses sceneData defaults. The initial front is horizontal along -position.x,
	 * rather than aimed at the origin; sceneData's starting x must be nonzero.
	 * Angles are in radians and sensitivity scales each movement/rotation input. */
	Camera();

	/* Starts at position and looks toward the world origin using normalize(-position).
	 * Preconditions: finite inputs; position is nonzero and not parallel to world Y;
	 * 0 < fovy < 180 degrees, aspect > 0, and 0 < near < far.
	 * Speeds, initial grounded state, and saved altitude come from sceneData.
	 * The initial saved altitude is cameraAltitude, not necessarily position.y. */
	Camera(const glm::vec3& position, const float& fovy, const float& aspect, const float& near, const float& far);
	
	// Setters.
	/* Sets the projection width/height ratio; aspect must be finite and positive. */
	void setAspect(float aspect) { m_aspect = aspect; }

	/* Converts the input to bool (zero is false); does not capture the current height.
	 * Only forward/backward movement restores the saved altitude. */
	void setGrounded(float grounded) { m_grounded = grounded; }

	/* Sets a finite eye position without changing orientation or saved altitude. */
	void setPosition(const glm::vec3& position) { m_position = position; }

	/* Requires a finite unit vector not parallel to world Y. Stores it unchanged,
	 * rebuilds the right/up basis, and captures the current position.y as altitude.
	 * Recomputes yaw/pitch in radians, clamping pitch to +/-89.9 degrees without
	 * applying that clamp to the stored front until a later mouse update. */
	void setFront(const glm::vec3& front) { m_front = front; updateCameraVectors(false); }


	// Getters.
	bool getGrounded() const { return m_grounded; }

	/* Returns the OpenGL perspective matrix; constructor fovy is converted from degrees. */
	glm::mat4 getPerspectiveProjMatrix() const;

	/* Maps viewport [0, width] x [0, height] to clip-space [-1, 1] x [-1, 1].
	 * Width and height must be positive; the fixed near/far planes are -1 and 1. */
	glm::mat4 getOrthoProj(const int& width, const int& height) const;

	/* Returns a view using eye=position, target=position+front, and the saved up basis. */
	glm::mat4 getViewMatrix() const;

	/* Returns camera position.*/
	glm::vec3 getPosition() const { return m_position; }

	/* Returns camera front. */
	glm::vec3 getFront() const { return m_front; }

	/* Returns target = position + front. */
	glm::vec3 getTarget() const { return m_position + m_front; }

	/* Orientation angles in radians. Standalone rotations can leave the basis stale. */
	float getYaw() const { return m_yaw; }
	float getPitch() const { return m_pitch; }


	/* Returns mouse sensitivity. */
	float getMouseSpeed() const { return m_mouseSpeed; }
	/* Returns keyboard sensitivity. */
	float getKeyboardSpeed() const { return m_keyboardSpeed; }

	


	// Movement inputs must be finite; displacement = keyboardSpeed * basis * dx.
	// Signed dx reverses direction. Forward/backward restore altitude if grounded;
	// right/left/up/down do not enforce grounding, and up/down use camera-local up.
	void moveForward(float dx) { m_position += m_keyboardSpeed * m_front * dx; if (m_grounded) m_position.y = m_altitude; }
	void moveBackward(float dx) { m_position -= m_keyboardSpeed * m_front * dx; if (m_grounded) m_position.y = m_altitude; }
	
	void moveRight(float dx) { m_position += m_keyboardSpeed * m_right * dx; }
	void moveLeft(float dx) { m_position -= m_keyboardSpeed * m_right * dx; }
	void moveUp(float dx) { m_position += m_keyboardSpeed * m_up * dx; }
	void moveDown(float dx) { m_position -= m_keyboardSpeed * m_up * dx; }

	/* Subtracts finite offX * mouseSpeed from yaw (radians), without updating vectors. */
	void rotateOnXAxis(float offX) { m_yaw -= offX * m_mouseSpeed;}
	/* Adds finite offY * mouseSpeed to pitch (radians), without clamping or updating
	 * vectors. Call processMouseInputs to synchronize the movement/view basis. */
	void rotateOnYAxis(float offY) { m_pitch += offY * m_mouseSpeed; }

	/* Applies finite mouse offsets, clamps pitch to +/-89 degrees, rebuilds all basis
	 * vectors, and captures position.y as altitude. Zero offsets also synchronize
	 * angles changed by standalone rotations; yaw is not wrapped. */
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
	 * Expressed in degrees, must be in the open interval (0, 180). */
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

	/* If true, only forward/backward movement restores the saved altitude height. */
	bool m_grounded;
	float m_altitude;

	/* Rebuilds right=normalize(cross(front, worldUp)), up=cross(right, front), and
	 * captures position.y. If true, derives a normalized front from yaw/pitch;
	 * if false, derives yaw/pitch from the caller-supplied unit front. */
	void updateCameraVectors(bool frontNeedsUpdate = true);


};