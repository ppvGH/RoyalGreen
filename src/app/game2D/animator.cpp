#include "animator.h"

#include <iostream>
Animator::Animator(int sheetRows, int sheetColumns) :
	m_currentFrame(0),
	m_currentAnimation(0),
	m_startFrame(0),
	m_endFrame(0),
	m_sheetRows(sheetRows),
	m_sheetColumns(sheetColumns),
	m_timePerFrame(0.0f),
	m_currentFrameTime(0.0f)
{
}

glm::vec4 Animator::updateUV(float dt, bool facingRight)
{

	m_currentFrameTime += dt;

	/* When a frame duration expires, the animation advance to the next frame;
	 * If the current frame is the endFrame, the animation is reset and starts again. */
	if (m_currentFrameTime >= m_timePerFrame)
	{
		m_currentFrameTime = 0.0f;
		m_currentFrame = (++m_currentFrame > m_endFrame) ? m_startFrame : m_currentFrame;
	}

	return computeUV(m_currentAnimation, m_currentFrame, facingRight);
}


void Animator::setAnimation(int start, int end, int line, float fps)
{
	/* Start and finish position on the animation line. */
	m_startFrame = start;
	m_endFrame = end;

	/* Duration of the frame (in seconds per frame) */
	m_timePerFrame = 1 / fps;

	/* Set current position on the spritesheet. */
	m_currentAnimation = line;
	m_currentFrame = start;
	m_currentFrameTime = 0.0f;
}


glm::vec4 Animator::computeUV(int row, int col, bool facingRight) const
{
	/* Row = 0, col = 0 is the right bottom corner of the spritesheet.
	 * (x,y) = (u_min, v_min) --- (z,w) (u_max, v_max). */

	float x = (col - 1) / static_cast<float>(m_sheetColumns);
	float y = 1 - row / static_cast<float>(m_sheetRows);
	float z = col / static_cast<float>(m_sheetColumns);
	float w = 1 - (row - 1) / static_cast<float>(m_sheetRows);


	/* If character is facing right return standard UV,
	 * otherwise swap U coordinates (x <--> z). */
	if (facingRight) return glm::vec4(x, y, z, w);
	else return glm::vec4(z, y, x, w);


}
