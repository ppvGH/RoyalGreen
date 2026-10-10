#pragma once
#include <string>
#include <filesystem>
#define SHADERS_DIR "res/shaders/"
#define MESHES_DIR "res/meshes/"
#define TEXTURE_DIR "res/textures/"

/* Relative defaults support preflight. main calls configure() with the
 * executable-relative or explicitly requested asset root before loading. */
namespace Path
{
	// #############################################################################################
	// #######################################  Shaders  ###########################################
	// #############################################################################################

	inline std::string pathBlinnPhongVert = std::string(SHADERS_DIR) + "blinnPhong.vert";
	inline std::string pathBlinnPhongFrag = std::string(SHADERS_DIR) + "blinnPhong.frag";
	inline std::string pathCRTFrag = std::string(SHADERS_DIR) + "CRT.frag";
	inline std::string pathAimVert = std::string(SHADERS_DIR) + "aim.vert";
	inline std::string pathAimFrag = std::string(SHADERS_DIR) + "aim.frag";
	inline std::string pathGameVert = std::string(SHADERS_DIR) + "game.vert";
	inline std::string pathGameFrag = std::string(SHADERS_DIR) + "game.frag";
	inline std::string pathPointDepthVert = std::string(SHADERS_DIR) + "pointDepth.vert";
	inline std::string pathPointDepthGeom = std::string(SHADERS_DIR) + "pointDepth.geom";
	inline std::string pathPointDepthFrag = std::string(SHADERS_DIR) + "pointDepth.frag";
	inline std::string pathSpotDepthVert = std::string(SHADERS_DIR) + "spotDepth.vert";
	inline std::string pathSpotDepthFrag = std::string(SHADERS_DIR) + "spotDepth.frag";

	// #############################################################################################
	// ########################################  Models  ###########################################
	// #############################################################################################

	inline std::string pathModel = std::string(MESHES_DIR) + "tempArcade.obj";
	inline std::string pathRoom = std::string(MESHES_DIR) + "new_room.obj";
	inline std::string pathLamp = std::string(MESHES_DIR) + "lamp.obj";
	inline std::string pathMainLamp = std::string(MESHES_DIR) + "mainLamp.obj";
	inline std::string pathPool = std::string(MESHES_DIR) + "pool.obj";
	inline std::string pathTableLamps = std::string(MESHES_DIR) + "tableLamps.obj";


	// #############################################################################################
	// ########################################  Textures  #########################################
	// #############################################################################################

	// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  3D scene  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*	
	// testing
	inline std::string pathRospi = std::string(TEXTURE_DIR) + "rospi.png";
	// room
	inline std::string pathFloorTile = std::string(TEXTURE_DIR) + "floorTile.png";


	// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  2D game  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
	// background
	inline std::string pathBackground = std::string(TEXTURE_DIR) + "background.png";
	// player
	inline std::string pathPlayerSprite = std::string(TEXTURE_DIR) + "player_spritesheet.png";
	inline std::string pathArrow = std::string(TEXTURE_DIR) + "arrow.png";
	// enemy
	inline std::string pathCatSprite = std::string(TEXTURE_DIR) + "cat_spritesheet.png";
	inline std::string pathEnergyBall = std::string(TEXTURE_DIR) + "energyBall.png";
	// HUD
	inline std::string pathCatLife = std::string(TEXTURE_DIR) + "catLife.png";

    // Configure once before constructing resource users. All paths are UTF-8.
    inline void configure(const std::filesystem::path& root) {
        pathBlinnPhongVert = (root / "shaders" / "blinnPhong.vert").generic_u8string();
        pathBlinnPhongFrag = (root / "shaders" / "blinnPhong.frag").generic_u8string();
        pathCRTFrag = (root / "shaders" / "CRT.frag").generic_u8string();
        pathAimVert = (root / "shaders" / "aim.vert").generic_u8string();
        pathAimFrag = (root / "shaders" / "aim.frag").generic_u8string();
        pathGameVert = (root / "shaders" / "game.vert").generic_u8string();
        pathGameFrag = (root / "shaders" / "game.frag").generic_u8string();
        pathPointDepthVert = (root / "shaders" / "pointDepth.vert").generic_u8string();
        pathPointDepthGeom = (root / "shaders" / "pointDepth.geom").generic_u8string();
        pathPointDepthFrag = (root / "shaders" / "pointDepth.frag").generic_u8string();
        pathSpotDepthVert = (root / "shaders" / "spotDepth.vert").generic_u8string();
        pathSpotDepthFrag = (root / "shaders" / "spotDepth.frag").generic_u8string();
        pathModel = (root / "meshes" / "tempArcade.obj").generic_u8string();
        pathRoom = (root / "meshes" / "new_room.obj").generic_u8string();
        pathLamp = (root / "meshes" / "lamp.obj").generic_u8string();
        pathMainLamp = (root / "meshes" / "mainLamp.obj").generic_u8string();
        pathPool = (root / "meshes" / "pool.obj").generic_u8string();
        pathTableLamps = (root / "meshes" / "tableLamps.obj").generic_u8string();
        pathRospi = (root / "textures" / "rospi.png").generic_u8string();
        pathFloorTile = (root / "textures" / "floorTile.png").generic_u8string();
        pathBackground = (root / "textures" / "background.png").generic_u8string();
        pathPlayerSprite = (root / "textures" / "player_spritesheet.png").generic_u8string();
        pathArrow = (root / "textures" / "arrow.png").generic_u8string();
        pathCatSprite = (root / "textures" / "cat_spritesheet.png").generic_u8string();
        pathEnergyBall = (root / "textures" / "energyBall.png").generic_u8string();
        pathCatLife = (root / "textures" / "catLife.png").generic_u8string();
    }
}