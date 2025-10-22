#pragma once

#include "../graphics/model.h"

class Room
{
public:

	/* No default constructor allowed. */
	Room() = delete;

	/* Creates a Room object from its filepath and a texture for its floor if given. */
	Room(const std::string& pathModel, Texture* floorTex = nullptr);

	/* Set model matrix. */
	void setWCSPosition() const { m_model.setWCSPosition(); }
	
	/* Overrides floor material with a texture. */
	void setFloor(const Texture& texture);

	/* Draws the model. */
	void draw() const { m_model.draw(); }

	/* Model getter (non-const). */
	Model& getModel() { return m_model; }
	/* Model getter (read-only) .*/
	const Model& getModel() const { return m_model; }

private:

	Model m_model;

	Texture* m_floorTex;

};