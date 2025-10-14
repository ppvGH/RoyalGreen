#pragma once
#include <string>

/* Paths to shader source files.
 * <X>_DIR are in the CMakeLists.txt, note that they end with a / character.
 * SHADERS_DIR = "${CMAKE_SOURCE_DIR}/res/shaders/"
 * MESHES_DIR = "${CMAKE_SOURCE_DIR}/res/meshes/" */
namespace Path
{
	inline const std::string pathVert = std::string(SHADERS_DIR) + "basic.vert";
	inline const std::string pathFrag = std::string(SHADERS_DIR) + "basic.frag";
	inline const std::string path2DVert = std::string(SHADERS_DIR) + "basic2D.vert";
	inline const std::string path2DFrag = std::string(SHADERS_DIR) + "basic2D.frag";
	inline const std::string pathTex2DVert = std::string(SHADERS_DIR) + "tex2D.vert";
	inline const std::string pathTex2DFrag = std::string(SHADERS_DIR) + "tex2D.frag";

	inline const std::string pathModel = std::string(MESHES_DIR) + "tempArcade.obj";
	inline const std::string pathRoom = std::string(MESHES_DIR) + "new_room.obj";
	inline const std::string pathLamp = std::string(MESHES_DIR) + "lamp.obj";

	inline const std::string pathRospi = std::string(TEXTURE_DIR) + "rospi.png";
	inline const std::string pathFloorTile = std::string(TEXTURE_DIR) + "floorTile.png";


	inline const std::string pathPlayerSprite = std::string(TEXTURE_DIR) + "player_spritesheet.png";
	inline const std::string pathBackground = std::string(TEXTURE_DIR) + "background.png";
}