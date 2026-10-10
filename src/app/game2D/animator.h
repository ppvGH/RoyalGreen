#pragma once
#include <glm/glm.hpp>
// Sprite-sheet clips use 1-based columns and rows, with row 1 at the top.
class Animator {
public:
    Animator(int rows, int columns);
    // Retains fractional elapsed time and processes all crossed frames.
    glm::vec4 updateUV(float dt, bool facingRight);
    void setAnimation(int start, int end, int row, float fps, bool loop=true);
    bool isAnimationFinished() const { return m_configured && m_currentFrame==m_endFrame; }
    int getFrame() const { return m_currentFrame; }
    // Includes the current frame and any frame crossed by the last update;
    // lets gameplay deliver shoot events even after a slow frame.
    bool passedFrame(int frame) const;
private:
    int m_currentFrame=0, m_currentAnimation=0, m_startFrame=0, m_endFrame=0;
    int m_sheetRows, m_sheetColumns, m_previousFrame=0;
    double m_timePerFrame=0, m_currentFrameTime=0, m_steps=0;
    bool m_loop=true, m_configured=false;
    glm::vec4 computeUV(int row, int col, bool facingRight) const;
};
