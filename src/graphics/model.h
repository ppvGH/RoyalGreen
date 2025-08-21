#pragma once

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<unordered_map>

#include "mesh.h"
#include "material.h"



class Model
{
public:
	/* Meshes container. */
	std::vector<Mesh> m_meshes;
	/* Materials container. */
	std::vector<Material> m_materials;

	/* Main function to populate m_meshes and m_materials. */
	void loadModel(const std::string& filepath);

	
	Mesh& getMesh(const std::string& name);

	void draw(Shader& shader) const;

private:

	/* Recursively passes through all nodes of the graph. */
	void processNode(aiNode* node, const aiScene* scene);
	/* Populates the Mesh objects inside m_meshes and sets up their VAOs, VBOs, EBOs. */
	Mesh processMesh(aiMesh* mesh, aiNode* node);
	/* Has to be called before processNode in loadModel. */
	void loadMaterial(const aiScene* scene);

	/* O(1) */
	std::unordered_map<std::string, Mesh*> m_meshMap;

};