#pragma once

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "mesh.h"
#include "material.h"



class Model
{
public:
	/* Meshes container. */
	std::vector<Mesh> m_meshes;
	/* Materials container. */
	std::vector<Material> m_materials;

	void loadModel(const std::string& filepath);

	void draw(Shader& shader) const;

private:

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, aiNode* node, const aiScene* scene);
	void loadMaterial(const aiScene* scene);



};