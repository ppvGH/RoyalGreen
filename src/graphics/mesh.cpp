#include "mesh.h"
#include <glad/glad.h>

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	// Layout location 2: texCoords.
	// Pointer position is computed by offsetof macro.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
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
