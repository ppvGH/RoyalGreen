#pragma once

/** @file
 * CPU geometry used by arcade picking. Rays and triangles must use the same
 * object coordinate system so model transforms do not distort the hit test.
 */


#include <glm/glm.hpp>
#include <vector>
#include "../graphics/vertex.h"

// #########################################################################
// ###########################  Triangle Data  #############################
// #########################################################################

/** Precomputed triangle edges for repeated picking queries.
 * The first vertex is borrowed: keep vertices alive and do not reallocate or
 * modify their positions while this object is used. Indices must be valid and
 * the triangle non-degenerate; otherwise construction is outside the contract.
 */
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

/** Parametric ray R(t) = origin + t * direction.
 * A finite, nonzero direction is required. It is not normalized automatically:
 * t is a parameter and becomes a distance only for a unit direction.
 */
struct Ray
{
	glm::vec3 m_origin;

	glm::vec3 m_direction;

	Ray(const glm::vec3& origin, const glm::vec3& direction);
};


// #########################################################################
// ######################     Geometry functions    ########################
// #########################################################################

/** Test a two-sided ray/triangle hit with the Moller-Trumbore algorithm.
 * @param localRay Ray transformed into the triangle's object coordinates.
 * @param triangleData Precomputed, non-degenerate triangle with live vertices.
 * @param tOut Written only on success; its previous value survives a miss.
 * @return True for barycentric coordinates inside the triangle, including edges
 * within sceneData::intersectionEpsilon (1e-6). Near-zero determinants and hits
 * behind the origin beyond that tolerance are rejected. A small negative t
 * within the tolerance is accepted by the existing implementation.
 */
bool mollerTrumbore(const Ray& localRay, const TriangleData& triangleData, float& tOut);