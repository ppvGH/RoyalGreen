#pragma once

#include <glm/glm.hpp>

class Animator
{
public:

	/* Requires positive sheetRows and sheetColumns. Initially frame/line/range are
	 * zero sentinels; configure a valid animation before requesting UV coordinates. */
	Animator(int sheetRows, int sheetColumns);

	/* Requires a configured animation and finite, nonnegative dt in seconds.
	 * Accumulates dt; reaching the duration advances exactly one frame and resets
	 * the accumulator to zero, discarding excess time even for a large dt.
	 * Frames loop from end back to start. UVs are (left, bottom, right, top);
	 * facingRight=false swaps only the U coordinates to mirror horizontally. */
	glm::vec4 updateUV(float dt, bool facingRight);

	/* Uses 1-based indices: 1 <= start <= end <= sheetColumns, and
	 * 1 <= line <= sheetRows; line 1 is the top row. fps must be finite and positive
	 * with a representable positive frame duration. Resets to start and clears
	 * accumulated time, including when selecting the same animation again. */
	void setAnimation(int start, int end, int line, float fps);

	/* True while currentFrame == endFrame, not a latched completion flag: the next
	 * timed update loops. Also true for the initial zero sentinels and a single frame. */
	bool isAnimationFinished() const { return m_currentFrame == m_endFrame; }

	/* Returns the 1-based current column after configuration, or zero beforehand. */
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

	/* Uses 1-based row/column indices with row 1 at the top; returns bottom-left
	 * (xy) and top-right (zw), swapping x/z when facing left. */
	glm::vec4 computeUV(int row, int col, bool facingRight) const;


};