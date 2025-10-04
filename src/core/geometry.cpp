#include "geometry.h"
#include "../app/scene_data.h"
#include <iostream>
#include "utilities.h"
// #########################################################################
// ###########################  Triangle Data  #############################
// #########################################################################

TriangleData::TriangleData(unsigned int iA, unsigned int iB, unsigned int iC, const std::vector<Vertex>& vertices) :
	m_iA(iA),
	m_iB(iB),
	m_iC(iC),
	m_vA(vertices[iA].getPosition()),				 
	m_e1(vertices[iB].getPosition() - m_vA),
	m_e2(vertices[iC].getPosition() - m_vA),
	m_normal(glm::normalize(glm::cross(m_e1, m_e2)))

{
}

// #########################################################################
// #############################     Ray     ###############################
// #########################################################################

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) :
	m_origin(origin),
	m_direction(direction)
{
}

// #########################################################################
// ######################     Geometry functions    ########################
// #########################################################################

/* Conventions: 
 > CAPS for vectors
 > lows for scalars
 > '*'  for scalar product
 > 'x'  for vector product
 > ''   for scalar by vector product 
 > '^T' for transposition
 > '<>' for matrices
 */
/* MOLLER - TRUMBORE ALGORITHM
 *
 > Ray:		R(t) = O + tD ;
 > Plane:	S(u,v) = A + uE1 + vE2 ;
 > System:	A + uE1 + vE2 = O + tD ;
 >			-tD + uE1 + vE2 = O - A ;
 >			[-D, E1, E2] * [t, u, v]^T = O - A ;
 > the determinant of the matrix <M> = [-D, E1, E2] is equal to the triple product:
 > det(<M>) = det = (E2 x -D) * E1 = (D x E2) * E1 = P * E1 ;
 > _1_ if |det| is ~0 ==> R is parallel to S ==> no intersection ;
 > _2_ let T = O - A, invdet = det^-1 ==> u = (T*P)invdet; if u not€[0,1] ==> no hit ;
 > _3_ let Q = T x E1 ==> v = (D*Q)invdet; if v not€ [0, 1-u] ==> no hit ;
 > _4_ t = (E2*Q)invdet; if t<0 no hit;
 > _5_ if t>=0 ==> HIT (t = t_h), point of intersection H = R(t_h) = O + Dt_h ;
 *
 */

/* TODO: funzione in geometry -> funzione in mesh -> funzione in model (dove c'è modelMat)*/

/* NOTA BENE: Ray O, D must be in same CS of the triangle, (i.e. of the mesh, i.e. of the model object), 
 * so these vectors must be multiplied by the inverse of the model matrix of the model object to 
 * get back to the LOCAL CS (object CS) = OCS. */
bool mollerTrumbore(const Ray& localRay, const TriangleData& triangleData, float& tOut)
{
	// Tollerance.
	const float eps = sceneData::intersectionEpsilon;

	const glm::vec3& O = localRay.m_origin;
	const glm::vec3& D = localRay.m_direction;

	const glm::vec3& A = triangleData.m_vA;
	const glm::vec3& E1 = triangleData.m_e1;
	const glm::vec3& E2 = triangleData.m_e2;
	/*std::cout << "origin:    ";  printVec3(O);
	std::cout << "direction: ";  printVec3(D);
	std::cout << "vertex A:  ";  printVec3(A);
	std::cout << "vector E1: ";  printVec3(E1);
	std::cout << "vector E2: ";  printVec3(E2);*/

	// 1st step
	glm::vec3 P = glm::cross(D, E2);
	float det = glm::dot(P, E1);
	
	if (std::abs(det) < eps) return false;

	// 2nd step
	float invdet = 1.0f / det;
	glm::vec3 T = O - A;
	float u = invdet * glm::dot(T, P);

	if (u < 0.0f - eps || u > 1.0f + eps) return false;		
	
	// 3rd step
	glm::vec3 Q = glm::cross(T, E1);
	float v = invdet * glm::dot(D, Q);

	if (v < 0.0f - eps || u + v > 1.0f + eps) return false;
	
	// 4th step
	float t = invdet * glm::dot(E2, Q);

	if (t < 0.0f - eps) return false;

	// 5th step : algorithm passed all checks
	// store the t
	tOut = t;
	return true;
}
