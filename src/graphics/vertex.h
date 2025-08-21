#pragma once

#include <glm/glm.hpp>

/* Basic struct representing a vertex passed to the GPU. */
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

/* Struct over class choice is for simplicity and because of
 * the usage of the "offsetof" macro in the Mesh setup method. 
 * Classes can give undefined behaviours with the offsetof macro,
 * even if in this particular case it could still work correctly,
 * as it is a POD (Plain Old Data) class. */