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

	/* Draws all meshes. */
	void draw(Shader& shader) const;

	/* Draws all meshes besides the one indicated. */
	void drawExclude(const std::string name, Shader& shader) const;

private:

	/* Recursively passes through all nodes of the graph. */
	void processNode(aiNode* node, const aiScene* scene);
	/* Populates the Mesh objects inside m_meshes and sets up their VAOs, VBOs, EBOs. */
	Mesh processMesh(aiMesh* mesh, aiNode* node);
	/* Has to be called before processNode in loadModel. */
	void loadMaterial(const aiScene* scene);

	/* Map of the meshes of the model. 
	 * Key is the mesh's <node name>_<material name>.
	 * If node has just one material the key is just <node name>. */
	std::unordered_map<std::string, Mesh*> m_meshMap;

};