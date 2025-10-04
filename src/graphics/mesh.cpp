#include "mesh.h"
#include <glad/glad.h>

Mesh::Mesh() :
	m_VAO(0),
	m_VBO(0),
	m_EBO(0),
	m_matIndex(-1)
{
}


void Mesh::setup()
{
	// Generate the Vertex Array Object.
	glGenVertexArrays(1, &m_VAO);
	// Generate the Vertex Buffer Object.
	glGenBuffers(1, &m_VBO);
	// Generate the Element Array Object.
	glGenBuffers(1, &m_EBO);

	// Bind the VAO.
	glBindVertexArray(m_VAO);

	// Bind the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Buffer an array of vertex data into the VBO; 
	// vec.data() points to the first element of vec, and if it's an empty vec it becomes a nullptr.
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW); 

	// Bind the EBO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	// Buffer an array of vertex indices (elements) into the EBO.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// Layout location 0: position.
	// Pointer is at the beginning of the data.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Layout location 1: normal.
	// Pointer position is computed by offsetof macro.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_normal));
	glEnableVertexAttribArray(1);

	// Layout location 2: texCoords.
	// Pointer position is computed by offsetof macro.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::m_texCoords));
	glEnableVertexAttribArray(2);

	// Unbind the Vertex Array.
	glBindVertexArray(0);

}

void Mesh::draw() const
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0); // not really necessary
}

glm::vec3 Mesh::getCenter() const
{
	/* Average of vertices positions. */
	glm::vec3 center = glm::vec3(0.0f);
	for (const auto& vert : m_vertices) center += vert.getPosition();
	center /= m_vertices.size();

	return center;
}

glm::vec3 Mesh::getGlobalNormal() const
{
	/* Normalized sum of the vertices normals. */
	glm::vec3 gNormal = glm::vec3(0.0f);
	for (const auto& vert : m_vertices)	gNormal += vert.getNormal();
	gNormal = glm::normalize(gNormal);

	return gNormal;
}


bool Mesh::intersectRayTriangle(const Ray& localRay, float& tOut) const
{
	bool hit = false;
	float tMin = std::numeric_limits<float>::max();

	for (const auto& triData : m_triangleData)
	{
		float t;
		if (mollerTrumbore(localRay, triData, t))
		{
			if (t < tMin)
			{
				tMin = t;
				hit = true;
			}
		}
	}

	if (hit) tOut = tMin;
	return hit;
}
