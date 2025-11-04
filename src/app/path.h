#pragma once
#include <string>

/* Paths to shader source files.
 * <X>_DIR are in the CMakeLists.txt, note that they end with a / character.
 * SHADERS_DIR = "${CMAKE_SOURCE_DIR}/res/shaders/"
 * MESHES_DIR = "${CMAKE_SOURCE_DIR}/res/meshes/" */
namespace Path
{
	// #############################################################################################
	// #######################################  Shaders  ###########################################
	// #############################################################################################

	inline const std::string pathBlinnPhongVert = std::string(SHADERS_DIR) + "blinnPhong.vert";
	inline const std::string pathBlinnPhongFrag = std::string(SHADERS_DIR) + "blinnPhong.frag";
	inline const std::string pathCRTFrag = std::string(SHADERS_DIR) + "CRT.frag";
	inline const std::string pathAimVert = std::string(SHADERS_DIR) + "aim.vert";
	inline const std::string pathAimFrag = std::string(SHADERS_DIR) + "aim.frag";
	inline const std::string pathGameVert = std::string(SHADERS_DIR) + "game.vert";
	inline const std::string pathGameFrag = std::string(SHADERS_DIR) + "game.frag";
	inline const std::string pathPointDepthVert = std::string(SHADERS_DIR) + "pointDepth.vert";
	inline const std::string pathPointDepthGeom = std::string(SHADERS_DIR) + "pointDepth.geom";
	inline const std::string pathPointDepthFrag = std::string(SHADERS_DIR) + "pointDepth.frag";
	inline const std::string pathSpotDepthVert = std::string(SHADERS_DIR) + "spotDepth.vert";
	inline const std::string pathSpotDepthFrag = std::string(SHADERS_DIR) + "spotDepth.frag";

	// #############################################################################################
	// ########################################  Models  ###########################################
	// #############################################################################################

	inline const std::string pathModel = std::string(MESHES_DIR) + "tempArcade.obj";
	inline const std::string pathRoom = std::string(MESHES_DIR) + "new_room.obj";
	inline const std::string pathLamp = std::string(MESHES_DIR) + "lamp.obj";
	inline const std::string pathMainLamp = std::string(MESHES_DIR) + "mainLamp.obj";
	inline const std::string pathPool = std::string(MESHES_DIR) + "pool.obj";

	// #############################################################################################
	// ########################################  Textures  #########################################
	// #############################################################################################

	// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  3D scene  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*	
	// testing
	inline const std::string pathRospi = std::string(TEXTURE_DIR) + "rospi.png";
	// room
	inline const std::string pathFloorTile = std::string(TEXTURE_DIR) + "floorTile.png";


	// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  2D game  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	// background
	inline const std::string pathBackground = std::string(TEXTURE_DIR) + "background.png";
	// player
	inline const std::string pathPlayerSprite = std::string(TEXTURE_DIR) + "player_spritesheet.png";
	inline const std::string pathArrow = std::string(TEXTURE_DIR) + "arrow.png";
	// enemy
	inline const std::string pathCatSprite = std::string(TEXTURE_DIR) + "cat_spritesheet.png";
	inline const std::string pathEnergyBall = std::string(TEXTURE_DIR) + "energyBall.png";
	// HUD
	inline const std::string pathCatLife = std::string(TEXTURE_DIR) + "catLife.png";
}