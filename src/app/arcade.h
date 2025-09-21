#pragma once

#include "../graphics/model.h"

class Arcade
{
public:

	/* Initializes an Arcade obj through the default model constructor.*/
	Arcade();

	/* Creates an Arcade obj through the parametric model constructor. */
	Arcade(const std::string& pathModel);

	/* Loads the model. Necessary when the default constructor is called. */
	void loadModel(const std::string& pathModel) { m_model.loadModel(pathModel); }

	/* Overrides screen material with a texture. */
	void setScreen(const Texture& texture);
	/* Activates/deactivates the screen. */
	void screenSwitch();

	/* Draws the model. */
	void draw(Shader& shader) const { m_model.draw(shader); }

	/* Model getter.*/
	const Model& getModel() const { return m_model; }

private:

	Model m_model;

	bool m_screenIsON;
};