#pragma once

#include <glm/glm.hpp>

class Animator
{
public:

	/* Initializes member variables. */
	Animator(int sheetRows, int sheetColumns);

	/* Updates UV. */
	glm::vec4 updateUV(float dt, bool facingRight);

	/* Sets member variables. */
	void setAnimation(int start, int end, int line, float fps);

	/* Returns true when currentFrame == endFrame. */
	bool isAnimationFinished() const { return m_currentFrame == m_endFrame; }

	int getFrame() const { return m_currentFrame; }

private:

	int m_currentFrame;			// col
	int m_currentAnimation;		// row/line

	int m_startFrame;
	int m_endFrame;

	/* Size in numbers of frames of the sprite sheet. */
	int m_sheetRows, m_sheetColumns;

	/* Duration in seconds of a single frame. */
	float m_timePerFrame;
	float m_currentFrameTime;

	/* Returns a vec4 with the 2 uv couples: botleft	(xy) topright (zw).
	 * These corresponds to the lower left and higher right corner of the frame. */
	glm::vec4 computeUV(int row, int col, bool facingRight) const;


};