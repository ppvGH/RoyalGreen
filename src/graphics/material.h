#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../graphics/shader.h"
#include "../graphics/texture.h"

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
	/* Shiny factor. */
	float m_shininess; 

	/* Override texture pointer (cause of nullptr). */
	const Texture* m_overrideTex;
	bool m_useTex;

	/* Method to set uniforms in the shader. */
	void apply(Shader& shader) const;

	/* Default constructor. */
	Material(): 
		m_matName(""),
		m_ambient(glm::vec3(0.0f)),
		m_diffuse(glm::vec3(0.0f)),
		m_specular(glm::vec3(0.0f)),
		m_emission(glm::vec3(0.0f)),
		m_shininess(0.0f),
		m_overrideTex(nullptr),
		m_useTex(false)
	{
	}
	
};


