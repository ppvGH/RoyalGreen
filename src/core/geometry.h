#pragma once


#include <glm/glm.hpp>
#include <vector>
#include "../graphics/vertex.h"

// #########################################################################
// ###########################  Triangle Data  #############################
// #########################################################################

struct TriangleData
{
	/* Index of the triangle in the mesh. */
	unsigned int m_iA, m_iB, m_iC;

	/* Vertex A. */
	const glm::vec3& m_vA;
	/* e1 = B - A;	 e2 = C - A. */
	glm::vec3 m_e1, m_e2;

	/* normal = norm(cross(e1,e2)). */
	glm::vec3 m_normal;

	/* Parametric constructor. Takes vertices from a mesh to setup e1, e2, normal. */
	TriangleData(unsigned int iA, unsigned int iB, unsigned int iC, const std::vector<Vertex>& vertices);
};


// #########################################################################
// #############################     Ray     ###############################
// #########################################################################

struct Ray
{
	glm::vec3 m_origin;

	glm::vec3 m_direction;

	Ray(const glm::vec3& origin, const glm::vec3& direction);
};


// #########################################################################
// ######################     Geometry functions    ########################
// #########################################################################

/* It takes as input a Ray already transformed in OCS through the inverse of the modelMatrix. */
bool mollerTrumbore(const Ray& localRay, const TriangleData& triangleData, float& tOut);