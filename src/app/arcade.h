#pragma once

#include "../graphics/model.h"

class Arcade
{
public:

	/* No default constructor allowed.*/
	Arcade() = delete;

	/* Creates an Arcade object from its filepath. */
	Arcade(const std::string& pathModel);

	/* Overrides screen material with a texture. */
	void setScreen(const Texture& texture);
	/* Activates/deactivates the screen. */
	void screenSwitch();

	/* Draws the model. */
	void draw(Shader& shader) const { m_model.draw(shader); }

	/* Model getter (non-const). */
	Model& getModel() { return m_model; }
	/* Model getter (read-only) .*/
	const Model& getModel() const { return m_model; }

private:

	Model m_model;

	bool m_screenIsON;
};