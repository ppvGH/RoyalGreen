#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../graphics/shader.h"
#include "../graphics/texture.h"

class Material
{
public:
	/* Name of the material (from the .obj file). */
	std::string m_matName;	

	/* Default constructor. */
	Material();

	/* Method to set uniforms in the shader. */
	void apply(Shader& shader) const;

	/* Setters. */
	void setUseTex(bool useTex) { m_useTex = useTex; }
	void setAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
	void setDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
	void setSpecular(const glm::vec3& specular) { m_specular = specular; }
	void setEmission(const glm::vec3& emission) { m_emission = emission; }
	void setShininess(float shininess) { m_shininess = shininess; }
	
	/* Texture overriding methods. */
	void toggleTex() { m_useTex = !m_useTex; }
	void overrideTex(const Texture& texture) { m_texture = &texture; }

private:

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
	const Texture* m_texture;
	/* Flag for overriding texture. */
	bool m_useTex;
};


