#include "material.h"

void Material::apply(Shader& shader) const
{
	/* Names of the uniforms should match ones in the shader. */
	shader.setVector3f("matAmbient", m_ambient);
	shader.setVector3f("matDiffuse", m_diffuse);
	shader.setVector3f("matSpecular", m_specular);
	shader.setVector3f("matEmission", m_emission);
	shader.setFloat("matShininess", m_shininess);


	if (m_useTex && m_overrideTex!=nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		m_overrideTex->bind();
		shader.setInt("useScreenTex", 1);
		shader.setInt("screenTex", 0);
	}
	else 
	{
		shader.setInt("useScreenTex", 0);
	}
}