#pragma once

#include "../graphics/model.h"

class Arcade
{
public:

	/* No default constructor allowed.*/
	Arcade() = delete;

	/* Creates an Arcade object from its filepath. */
	Arcade(const std::string& pathModel);

	/* Set model matrix. */
	void setWCSPosition() const { m_model.setWCSPosition(); }

	/* Overrides screen material with a texture. */
	void setScreen(const Texture& texture);

	/* Customized shader setter able to set CRT shader for the display if input is blinnPhong.*/
	void setShader(const std::string& shaderName);

	/* Activates/deactivates the screen. */
	void screenSwitch();

	/* Screen state getter. */
	bool isDisplayOn() const { return m_screenIsON; }

	/* Draws the model. */
	void draw() const { m_model.draw(); }

	/* Model getter (non-const). */
	Model& getModel() { return m_model; }
	/* Model getter (read-only) .*/
	const Model& getModel() const { return m_model; }

private:

	Model m_model;

	bool m_screenIsON;
};