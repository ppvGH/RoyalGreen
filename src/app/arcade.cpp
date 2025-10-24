#include "arcade.h"
#include "scene_data.h"
#include "../core/resource_manager.h"
#include <iostream>

Arcade::Arcade(const std::string& pathModel):
	m_model(pathModel, &ResourceManager::getShader("basic")),
	m_screenIsON(false)
{
	// when the texture is rendered into the screen, dont mix the color with lighting model
	m_model.getMesh(sceneData::meshScreenName).m_mixTex = false;

}


void Arcade::setScreen(const Texture& texture)
{
	Material& matScreen = m_model.getMaterial(sceneData::matScreenName);
	matScreen.overrideTex(texture);
}

void Arcade::resetPhong()
{
	m_model.setShader("basic");
	if(m_screenIsON) m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader("CRT"));

}

void Arcade::screenSwitch()
{
	/* Inverts the state of the screen. */
	m_screenIsON = !m_screenIsON;

	/* Sets the shader associated with the display mesh. */
	if (m_screenIsON) m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader("CRT"));
	else m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader("basic"));

	/* Button turns glowing red when ON. */
	Material& matPowerButton = m_model.getMaterial(sceneData::matPowerButtonName);
	matPowerButton.setEmission(glm::vec3(float(m_screenIsON), 0.0f, 0.0f));

	/* Display material update. */
	Material& matScreen = m_model.getMaterial(sceneData::matScreenName);
	matScreen.setEmission(glm::vec3(m_screenIsON * 1.0f));

	/* Inverts the uniform in the phong shader to render the screen content. 
	 * Allows the sampling of the texture in the Material::apply method. */
	matScreen.toggleTex();

}
