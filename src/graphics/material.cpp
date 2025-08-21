#include "material.h"

void Material::apply(Shader& shader) const
{
	/* Names of the uniforms should match ones in the shader. */
	shader.setVector3f("matAmbient", m_ambient);
	shader.setVector3f("matDiffuse", m_diffuse);
	shader.setVector3f("matSpecular", m_specular);
	shader.setVector3f("matEmission", m_emission);
	shader.setFloat("matShininess", m_shininess);
}