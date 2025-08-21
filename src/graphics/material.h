#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../core/shader.h"

struct Material
{
	/* Name of the material (from the .obj file). */
	std::string m_matName;

	// Base vector colors.
	/* Ambiental light color[k_a]. */
	glm::vec3 m_ambient;
	/* Main color (albedo) [k_d]. */
	glm::vec3 m_diffuse;
	/* Color from specular reflexes [k_s]. */
	glm::vec3 m_specular;
	/* Self-lighting color [k_e]. */
	glm::vec3 m_emission;

	// Scalars.
	/* Shiny factor. */
	float m_shininess; 

	/* Method to set uniforms in the shader. */
	void apply(Shader& shader) const;
	
};


