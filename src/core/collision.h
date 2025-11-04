#pragma once

#include <glm/glm.hpp>

struct BoundingBox
{
	glm::vec2 m_min;
	glm::vec2 m_max;
	glm::vec2 m_size;

	/* Parametric constructor. */
	BoundingBox(glm::vec2 pos, glm::vec2 size);

	/* Update BB when position moves. */
	void updateBB(glm::vec2 pos);

};


/* Intersection between two bounding boxes.
 * When boxes overlaps it is always true that:
 * x1 < X2 && y1 < Y2 && X1 > x1 && Y2 > y2,
 * where 1 and 2 represents the indices for bb1 and bb2, and
 * (x,y) := min, (X,Y) := max. */
bool intersectBB(const BoundingBox& bb1, const BoundingBox& bb2);