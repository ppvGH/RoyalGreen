#include "resource_manager.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader& ResourceManager::loadShader(const std::string& pathVert, const std::string& pathFrag, const std::string& name)
{
	// loads vertex and fragment shader source code as two strings from filepaths
	std::string srcVert = loadFileToString(pathVert);
	std::string srcFrag = loadFileToString(pathFrag);

	// stores the shader with its name
	shadersMap[name].linkProgram(srcVert, srcFrag);;
	
	// returns the shader stored in the unordered map
	return shadersMap[name];
}

Shader& ResourceManager::GetShader(const std::string& name)
{
	// iterates through the map to find the shader with its name
	auto it = shadersMap.find(name);
	if (it != shadersMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "ERROR::RESOURCE_MANAGER::GetShader: Shader \"" << name << "\" not found." << std::endl;
		throw std::runtime_error("GetShader fail.");

	}
}



std::string ResourceManager::loadFileToString(const std::string& pathShaderStage)
{
	// read file from path
	std::ifstream fileShaderStage(pathShaderStage);
	if (!fileShaderStage.is_open())
	{
		std::cerr << "ERROR::SHADER:: File load failure " << std::endl;
		return "";
	}

	// create buffer
	std::stringstream bufferShaderStage;

	// stream content into buffer
	bufferShaderStage << fileShaderStage.rdbuf();

	// close file
	fileShaderStage.close();

	// returns buffer content as a string
	return bufferShaderStage.str();

}
