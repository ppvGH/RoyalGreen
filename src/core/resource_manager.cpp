#include "resource_manager.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h" 

Shader& ResourceManager::loadShader(const std::string& pathVert, const std::string& pathFrag, const std::string& name)
{
	// loads vertex and fragment shader source code as two strings from filepaths
	std::string srcVert = loadFileToString(pathVert);
	std::string srcFrag = loadFileToString(pathFrag);

	// stores the shader with its name
	shadersMap[name].linkProgram(srcVert, srcFrag);
	
	// returns the shader stored in the unordered map
	return shadersMap[name];
}

Texture& ResourceManager::loadTexture(const std::string& pathTex, TexParams& params, const std::string& name)
{
	const char* path = pathTex.c_str();

	/* Retrieve data from stbi. */
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);	// flip needed
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data)
	{
		/* Correct parameters for the texture retrieved from data.*/
		if (params.m_width == 0) params.m_width = width;
		if (params.m_height == 0) params.m_height = height;

		GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
		params.m_format = format;

		/* Storing the texture in the map. */
		texturesMap[name] = Texture(params, data);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture: " << path << std::endl;
		stbi_image_free(data);
	}

	return texturesMap[name];
}

Shader& ResourceManager::getShader(const std::string& name)
{
	// iterates through the map to find the shader with its name
	auto it = shadersMap.find(name);
	if (it != shadersMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "ERROR::RESOURCE_MANAGER::getShader: Shader \"" << name << "\" not found." << std::endl;
		throw std::runtime_error("GetShader failure.");
	}
}

Texture& ResourceManager::getTexture(const std::string& name)
{
	auto it = texturesMap.find(name);
	if (it != texturesMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "ERROR::RESOURCE_MANAGER::getTexture: Testure\"" << name << "\" not found." << std::endl;
		throw std::runtime_error("getTexture failure.");
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
