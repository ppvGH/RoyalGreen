#pragma once

#include <vector>
#include <string>

#include "vertex.h"
#include "../core/geometry.h"

/* Class responsible for a single mesh setup and draw. */
class Mesh 
{
public:
	/* Name of the mesh (from the .obj file). */
	std::string m_meshName;
	/* Mesh vertices passed to the GPU. */
	std::vector<Vertex> m_vertices;
	/* Needed for DrawElements. */
	std::vector<unsigned int> m_indices;
	/* Associates the mesh with a material stored in the model.
	 * Initialized at -1 in the default constructor to avoid wrong assignments. */
	int m_matIndex;
	/* Needed for ray-triangle intersection. */
	std::vector<TriangleData> m_triangleData;

	/* Default constructor. */
	Mesh();

	/* Initializes VAO, VBO, EBO. */
	void setup();

	/* Draws the mesh. */
	void draw() const;

	/* Computes the position of the mesh's center. */
	glm::vec3 getCenter() const;

	/* Computes the global normal of the mesh.
	 * NB: this is meaningful only with planar meshes. */
	glm::vec3 getGlobalNormal() const;

	bool intersectRayTriangle(const Ray& localRay, float& tOut) const;

	/* VAO getter. */
	unsigned int getVAO() const { return m_VAO; };

	void resizeUV(float UVresize) { m_UVresize = UVresize; }
	float m_UVresize;

private:
	/* Mesh identifier for the Vertex Array Object. */
	unsigned int m_VAO;
	/* Mesh identifier for the Vertex Buffer Object. */
	unsigned int m_VBO;
	/* Mesh identifier for the Element Buffer Object. */
	unsigned int m_EBO;




};