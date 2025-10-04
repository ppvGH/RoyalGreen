#pragma once

#include <glm/glm.hpp>

/* Basic struct representing a vertex passed to the GPU. */
struct Vertex
{
	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_texCoords;

	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getNormal() const { return m_normal; }
	const glm::vec2& getTexCoords() const { return m_texCoords; }

	Vertex() = delete;

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords) :
		m_position(position),
		m_normal(normal),
		m_texCoords(texCoords)
	{
	}


};

/* Struct over class choice is for simplicity and because of
 * the usage of the "offsetof" macro in the Mesh setup method. 
 * Classes can give undefined behaviours with the offsetof macro,
 * even if in this particular case it could still work correctly,
 * as it would be a POD (Plain Old Data) class. */