#include "collision.h"
#include "utilities.h"
#include <iostream>

BoundingBox::BoundingBox(glm::vec2 pos, glm::vec2 size):
	m_min(pos),
	m_max(pos + size),
	m_size(size)
{
}

void BoundingBox::updateBB(glm::vec2 pos)
{
	m_min = pos; 
	m_max = pos + m_size; 
}

bool intersectBB(const BoundingBox& bb1, const BoundingBox& bb2)
{
	/*std::cout << "Proj min: "; printVec2(bb1.m_min);
	std::cout << "Proj max: "; printVec2(bb1.m_max);
	std::cout << "Player min: "; printVec2(bb2.m_min);
	std::cout << "Player max: "; printVec2(bb2.m_max);
	std::cout << "x1(" << bb1.m_min.x << ") < X2(" << bb2.m_max.x << ") : " << (bb1.m_min.x < bb2.m_max.x) << std::endl;
	std::cout << "y1(" << bb1.m_min.y << ") < Y2(" << bb2.m_max.y << ") : " << (bb1.m_min.y < bb2.m_max.y) << std::endl;
	std::cout << "X1(" << bb1.m_max.x << ") < x2(" << bb2.m_min.x << ") : " << (bb1.m_max.x > bb2.m_min.x) << std::endl;
	std::cout << "Y1(" << bb1.m_max.y << ") < y2(" << bb2.m_min.y << ") : " << (bb1.m_max.y > bb2.m_min.y) << std::endl;
	std::cout << "------------------------------\n";*/

	return (bb1.m_min.x < bb2.m_max.x) && (bb1.m_min.y < bb2.m_max.y) &&	// x1 < X2 && y1 < Y2 &&
		(bb1.m_max.x > bb2.m_min.x) && (bb1.m_max.y > bb2.m_min.y);			// X1 > x2 && Y1 > y2
}
