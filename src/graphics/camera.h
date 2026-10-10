#pragma once
#include <glm/glm.hpp>

/** Movement uses elapsed seconds and scene units/second. FOV is degrees;
 * yaw/pitch and mouse sensitivity use radians. Invalid values throw
 * std::invalid_argument. The default camera faces horizontally toward -X. */
class Camera {
public:
    Camera();
    /** Look toward the origin. Position must be finite, nonzero and not parallel
     * to world Y; 0<fovy<180, aspect>0, 0<near<far. Saves position.y as altitude. */
    Camera(const glm::vec3& position,const float& fovy,const float& aspect,const float& near,const float& far);
    void setAspect(float aspect);
    /** Capture current height; nonzero enables forward/backward height locking. */
    void setGrounded(float grounded);
    /** Set a finite position and capture its height without changing direction. */
    void setPosition(const glm::vec3& position);
    /** Normalize nonzero direction, clamp pitch to +/-89 degrees, rebuild basis. */
    void setFront(const glm::vec3& front);
    void setKeyboardSpeed(float speed);
    void setMouseSpeed(float speed);
    bool getGrounded() const { return m_grounded; }
    glm::mat4 getPerspectiveProjMatrix() const;
    /** Positive dimensions; maps [0,width] x [0,height], near/far=-1/1. */
    glm::mat4 getOrthoProj(const int& width,const int& height) const;
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getFront() const { return m_front; }
    glm::vec3 getTarget() const { return m_position+m_front; }
    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }
    float getMouseSpeed() const { return m_mouseSpeed; }
    float getKeyboardSpeed() const { return m_keyboardSpeed; }
    /** Signed finite seconds permit reverse movement. Forward/backward lock Y
     * when grounded; up/down change saved altitude, right/left use local right.
     * These methods do not implement room collision. */
    void moveForward(float dt);
    void moveBackward(float dt);
    void moveRight(float dt);
    void moveLeft(float dt);
    void moveUp(float dt);
    void moveDown(float dt);
    /** Finite cursor offsets multiplied by sensitivity; immediately refresh basis. */
    void rotateOnXAxis(float offX);
    void rotateOnYAxis(float offY);
    void processMouseInputs(const float& offX,const float& offY);
private:
    glm::vec3 m_position,m_front;
    const glm::vec3 m_VUP=glm::vec3(0,1,0);
    glm::vec3 m_right,m_up;
    float m_yaw,m_pitch,m_fovy,m_aspect,m_near,m_far;
    float m_mouseSpeed,m_keyboardSpeed;
    bool m_grounded;
    float m_altitude;
    void updateCameraVectors(bool frontNeedsUpdate=true);
};
