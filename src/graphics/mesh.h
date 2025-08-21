#pragma once

#include <vector>
#include <string>

#include "vertex.h"


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
	 * Initialized at -1 to avoid wrong assignments. */
	int m_matIndex = -1;

	/* Initializes VAO, VBO, EBO. */
	void setup();

	/* Draws the mesh. */
	void draw() const;


private:
	/* Mesh identifier for the Vertex Array Object. */
	unsigned int m_VAO;
	/* Mesh identifier for the Vertex Buffer Object. */
	unsigned int m_VBO;
	/* Mesh identifier for the Element Buffer Object. */
	unsigned int m_EBO;



};