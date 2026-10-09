#pragma once

/** @file
 * Axis-aligned collision primitives in 2D game coordinates (pixels).
 * Sizes must be positive; constructors deliberately do not validate inputs.
 */

#include <glm/glm.hpp>

struct BoundingBox
{
	glm::vec2 m_min;
	glm::vec2 m_max;
	glm::vec2 m_size;

	/** Position is the lower-left corner; size is the positive width/height. */
	BoundingBox(glm::vec2 pos, glm::vec2 size);

	/** Move both corners while preserving the original size. */
	void updateBB(glm::vec2 pos);

};


/** Return true for strictly overlapping axis-aligned boxes.
 * Touching edges/corners do not collide: this avoids treating mere contact
 * as a projectile hit. No epsilon or geometric input validation is applied.
 */
bool intersectBB(const BoundingBox& bb1, const BoundingBox& bb2);