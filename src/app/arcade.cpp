#include "arcade.h"

Arcade::Arcade():
	m_model(),
	m_screenIsON(false)
{
}

Arcade::Arcade(const std::string& pathModel):
	m_model(pathModel),
	m_screenIsON(false)
{
}


void Arcade::setScreen(const Texture& texture)
{
	Material& matScreen = m_model.getMaterial("display");
	matScreen.overrideTex(texture);
}

void Arcade::screenSwitch()
{
	/* Inverts the state. */
	m_screenIsON = !m_screenIsON;

	/* Button turns glowing red when ON. */
	Material& matPowerButton = m_model.getMaterial("power_button_power_button");
	matPowerButton.setEmission(glm::vec3(float(m_screenIsON), 0.0f, 0.0f));

	/* Inverts the uniform in the shader to render the screen. */
	Material& matScreen = m_model.getMaterial("display");
	matScreen.toggleTex();
}
