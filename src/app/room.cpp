#include "room.h"
#include "scene_data.h"
#include "../core/resource_manager.h"

Room::Room(const std::string& pathModel, Texture* floorTex):
	m_model(pathModel, &ResourceManager::getShader("basic")),
	m_floorTex(floorTex)
{
	if (floorTex != nullptr) setFloor(*floorTex);
}




void Room::setFloor(const Texture& texture)
{
	/* Tiling setup: mesh UV resizing. */
	Mesh& meshFloor = m_model.getMesh(sceneData::meshFloorName);
	meshFloor.resizeUV(sceneData::floorTileFactor);

	/* Override material with an external texture. */
	Material& matFloor = m_model.getMaterial(sceneData::matFloorName);
	matFloor.overrideTex(texture);

	/* Texture is fixed and not switchable. */
	matFloor.setUseTex(true);
}
