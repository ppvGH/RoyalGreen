#include "arcade.h"
#include "scene_data.h"
#include "../../core/resource_manager.h"
#include <iostream>

Arcade::Arcade(const std::string& pathModel, Texture* yMinFaceTex):
	m_model(pathModel, &ResourceManager::getShader(sceneData::blinnPhongShaderName)),
	m_yMinFaceTex(yMinFaceTex),
	m_screenIsON(false)
{
	// when the texture is rendered into the screen, dont mix the color with lighting model
	m_model.getMesh(sceneData::meshScreenName).m_mixTex = false;

	// set y_min face material of the cabinet to the texture
	if (yMinFaceTex != nullptr) setArcadeFace(*yMinFaceTex);

}

void Arcade::setArcadeFace(const Texture& texture)
{
	/* Override material with an external texture. */
	Material& matYMinFace = m_model.getMaterial(sceneData::meshYMinFace);
	matYMinFace.overrideTex(texture);

	/* Texture is fixed and not switchable. */
	matYMinFace.setUseTex(true);
}


void Arcade::setScreen(const Texture& texture)
{
	Material& matScreen = m_model.getMaterial(sceneData::meshScreenName);
	matScreen.overrideTex(texture);
}

void Arcade::setShader(const std::string& shaderName)
{
	m_model.setShader(shaderName);
	bool condition = m_screenIsON && (shaderName == sceneData::renderShader);
	if(condition) m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader(sceneData::displayShader));
}

void Arcade::screenSwitch()
{
	/* Inverts the state of the screen. */
	m_screenIsON = !m_screenIsON;

	/* Sets the shader associated with the display mesh. */
	if (m_screenIsON) m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader(sceneData::displayShader));
	else m_model.getMesh(sceneData::meshScreenName).setShader(ResourceManager::getShader(sceneData::renderShader));

	/* Button turns glowing red when ON. */
	Material& matPowerButton = m_model.getMaterial(sceneData::meshPowerButtonName);
	matPowerButton.setEmission(sceneData::powerButtonEmission * glm::vec3(float(m_screenIsON), 0.0f, 0.0f));

	/* Display material update. */
	Material& matScreen = m_model.getMaterial(sceneData::meshScreenName);
	//matScreen.setEmission(glm::vec3(m_screenIsON * 1.0f));

	/* Inverts the uniform in the phong shader to render the screen content. 
	 * Allows the sampling of the texture in the Material::apply method.
	 * Indirectly binds the texture. */
	matScreen.toggleTex();

}
