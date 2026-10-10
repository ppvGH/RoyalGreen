#include "animator.h"
#include "../../core/validation.h"
#include <algorithm>
#include <cmath>
Animator::Animator(int rows,int columns):m_sheetRows(rows),m_sheetColumns(columns) {
    validation::require(rows>0 && columns>0,"Sprite sheet dimensions must be positive");
    setAnimation(1,1,1,1);
}
void Animator::setAnimation(int start,int end,int row,float fps,bool loop) {
    validation::require(start>=1 && start<=end && end<=m_sheetColumns && row>=1 && row<=m_sheetRows
        && validation::finite(fps) && fps>0,"Invalid sprite animation range, row or FPS");
    m_startFrame=start; m_endFrame=end; m_currentAnimation=row;
    m_previousFrame=m_currentFrame=start; m_timePerFrame=1.0/static_cast<double>(fps);
    m_currentFrameTime=0; m_steps=0; m_loop=loop; m_configured=true;
}
glm::vec4 Animator::updateUV(float dt,bool facingRight) {
    validation::time(dt);
    m_previousFrame=m_currentFrame;
    const double total=m_currentFrameTime+dt;
    m_steps=std::floor(total/m_timePerFrame+1e-6);
    m_currentFrameTime=std::max(0.0,total-m_steps*m_timePerFrame);
    if(m_loop) {
        const int count=m_endFrame-m_startFrame+1;
        const int steps=static_cast<int>(std::fmod(m_steps,static_cast<double>(count)));
        m_currentFrame=m_startFrame+(m_currentFrame-m_startFrame+steps)%count;
    } else {
        m_steps=std::min(m_steps,static_cast<double>(m_endFrame-m_currentFrame));
        m_currentFrame+=static_cast<int>(m_steps);
    }
    return computeUV(m_currentAnimation,m_currentFrame,facingRight);
}
bool Animator::passedFrame(int frame) const {
    if(frame<m_startFrame || frame>m_endFrame) return false;
    if(frame==m_currentFrame) return true;
    if(!m_loop) return frame>=m_previousFrame && frame<=m_currentFrame;
    const int count=m_endFrame-m_startFrame+1;
    const int distance=(frame-m_previousFrame+count)%count;
    return m_steps>=count || (distance>0 && distance<=m_steps);
}
glm::vec4 Animator::computeUV(int row,int col,bool right) const {
    const float x=(col-1)/static_cast<float>(m_sheetColumns), z=col/static_cast<float>(m_sheetColumns);
    const float y=1-row/static_cast<float>(m_sheetRows), w=1-(row-1)/static_cast<float>(m_sheetRows);
    return right?glm::vec4(x,y,z,w):glm::vec4(z,y,x,w);
}
