#include "material.h"

#include <glad/glad.h>
Material::Material() :
	m_matName(""),
	m_ambient(glm::vec3(0.0f)),
	m_diffuse(glm::vec3(0.0f)),
	m_specular(glm::vec3(0.0f)),
	m_emission(glm::vec3(0.0f)),
	m_shininess(0.0f),
	m_texture(nullptr),
	m_useTex(false)
{
}


void Material::apply(Shader& shader) const
{
	/* Names of the uniforms should match ones in the shader. */
	/* Controls on uniform location are due to Model::draw() method: if a shader doesn't have a uniform it shouldn't try to set it. */
	if (glGetUniformLocation(shader.getID(), "matAmbient") != -1) shader.setVector3f("matAmbient", m_ambient); 
	if (glGetUniformLocation(shader.getID(), "matDiffuse") != -1) shader.setVector3f("matDiffuse", m_diffuse);
	if (glGetUniformLocation(shader.getID(), "matSpecular") != -1) shader.setVector3f("matSpecular", m_specular);
	if (glGetUniformLocation(shader.getID(), "matEmission") != -1) shader.setVector3f("matEmission", m_emission);
	if (glGetUniformLocation(shader.getID(), "matShininess") != -1) shader.setFloat("matShininess", m_shininess);

	if (m_useTex && m_texture!=nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		m_texture->bind();
		if (glGetUniformLocation(shader.getID(), "useTex") != -1) shader.setInt("useTex", 1);
	}
	else if (glGetUniformLocation(shader.getID(), "useTex") != -1) shader.setInt("useTex", 0);
}

