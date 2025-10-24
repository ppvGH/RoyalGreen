#pragma once

#include<unordered_map>
#include<string>

#include "../graphics/shader.h"
#include "../graphics/texture.h"

/* Static singleton class to load shaders and textures.*/
class ResourceManager
{
public:

	/* Shader programs storage.
	 - key: logic name of the shader program
	 - value: shader object */
	inline static std::unordered_map<std::string, Shader> shadersMap;

	/* Textures storage.
	 - key: logic name of the texture
	 - value: texture object */
	inline static std::unordered_map<std::string, Texture> texturesMap;

	/* creates a shader program and stores it in the shadersMap 
	 - pathVert and pathFrag are the filepaths of vertex and fragment shader stages 
	 - name is the logic name of the shader program used as key in the shadersMap */
	static Shader& ResourceManager::loadShader(const std::string& pathVert, const std::string& pathFrag, const std::string& name);
	/* creates a shader program and stores it in the shadersMap
	 - pathVert, pathGeom, pathFrag are the filepaths of vertex, geometry and fragment shader stages
	 - name is the logic name of the shader program used as key in the shadersMap */
	static Shader& ResourceManager::loadShader(const std::string& pathVert, const std::string& pathGeom, const std::string& pathFrag, const std::string& name);

	/* Creates a texture and stores it in the texturesMap.
	 - pathTex is the filepath of the image file;
	 - params is a struct containing the wrap and filter parameters of the texture;
	 - name is the logic name of the texture object used as key in the texturesMap. */
	static Texture& ResourceManager::loadTexture(const std::string& pathTex, TexParams& params, const std::string& name);

	/* Object getters. */
	static Shader& ResourceManager::getShader(const std::string& name);
	static Texture& ResourceManager::getTexture(const std::string& name);


private:

	/* reads shader stage files from filepath */
	static std::string loadFileToString(const std::string& shaderStagePath);
};