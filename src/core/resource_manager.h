#pragma once

#include<unordered_map>
#include<string>

#include "shader.h"

/* Static singleton class to load shaders and textures.*/
class ResourceManager
{
public:

	/* shader programs storage.
	 - key: logic name of the shader program
	 - value: shader program id */
	inline static std::unordered_map<std::string, Shader> shadersMap;

	/* creates a shader program and stores it in the shadersMap 
	 - pathVert and pathFrag are the filepaths of vertex and fragment shader stages 
	 - name is the logic name of the shader program used as key in the shadersMap */
	static Shader& ResourceManager::loadShader(const std::string& pathVert, const std::string& pathFrag, const std::string& name);

	/* shader getter */
	static Shader& ResourceManager::GetShader(const std::string& name);

private:

	/* reads shader stage files from filepath */
	static std::string loadFileToString(const std::string& shaderStagePath);
};