#include "model.h"
#include <iostream>
#include <cassert>


Model::Model(const std::string& filepath):
	m_modelMat(1.0f)
{
	loadModel(filepath);
}


const Mesh& Model::getMesh(const std::string& name) const
{
	auto it = m_meshMap.find(name);
	if (it == m_meshMap.end())
	{
		std::cout << "getMesh error" << std::endl;
		throw std::runtime_error("Mesh \"" + name + "\" doesn't exist.");
	}
	return *(it->second);
}

Material& Model::getMaterial(const std::string& name)
{
	return this->m_materials[this->getMesh(name).m_matIndex];
}

Material& Model::getMaterial(const Mesh& mesh)
{
	return this->m_materials[mesh.m_matIndex];
}


void Model::draw(Shader& shader) const
{
	for (const auto& mesh : m_meshes)
	{
		if (mesh.m_matIndex > -1 && mesh.m_matIndex < m_materials.size())
		{
			/* Note that materialIndex = 0 is reserved for the Default Material. */
			m_materials[mesh.m_matIndex].apply(shader);
		}
		mesh.draw();
	}
}

//void Model::drawExclude(const std::string name, Shader& shader) const
//{
//	for (const auto& mesh : m_meshes)
//	{
//		if(mesh.m_meshName != name)
//		{
//			m_materials[mesh.m_matIndex].apply(shader);
//			mesh.draw();
//		}
//	}
//}


glm::vec3 Model::getWCSPosition() const
{
	/* v = (x,y,z,1) = (0, 0, 0, 1). */
	glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);
	/* v' = M*v = (x+tx, y+tx, z+tx, 1) = (tx, ty, tz, 1). */
	position = m_modelMat * position;
	/* Casting from vec4 to vec3. */
	return glm::vec3(position);
}

void Model::loadModel(const std::string& filepath)
{
	/* Importer reads the model file into a scene. */
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_Triangulate |							// meshes' polygons are decomposed into triangles
		aiProcess_JoinIdenticalVertices);// |				// reduce significantly memory space usage
		//aiProcess_FlipUVs);								// some models have the V coord flipped upside down (not needed for now)

	/* Error checker. */
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR ASSIMP: " << importer.GetErrorString() << std::endl;
		return;
	}

	/* Need to pupulate m_materials before processNode to ensure a correct 
	 * naming in the processMesh function. */
	loadMaterial(scene);

	/* Populate m_meshes. */
	processNode(scene->mRootNode, scene);

	/* Populates the map with the (reference of) last inserted mesh and its name. */
	for (auto& mesh : m_meshes) m_meshMap[mesh.m_meshName] = &mesh;

}


void Model::processNode(aiNode* node, const aiScene* scene)
{
	
	for (int nm = 0; nm < node->mNumMeshes; nm++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[nm]];
		m_meshes.push_back(processMesh(mesh, node));
	}
	
	for (int nc = 0; nc < node->mNumChildren; nc++)
	{
		processNode(node->mChildren[nc], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, aiNode* node)
{
	
	/* Temporary mesh object. */
	Mesh tMesh;
	/* Setup the material index of the temporary mesh with. */
	tMesh.m_matIndex = mesh->mMaterialIndex;

	/* Naming setup. */
	/* Assimp gives object (o) name to the first submesh and material (usemtl) name to the subsequent submeshes.
	 * Node name is always the o name, besides the root one that is named after the .obj filename. 
	 * Anyway, it doesn't even enter the processNode loop as it has mNumMeshes = 0. */
	tMesh.m_meshName = std::string(node->mName.C_Str());

	/* The first condition also ensures that m_materials is not empty. 
	 * In the second condition the material suffix is useful only if the node has 2 or more meshes. */
	if ( (mesh->mMaterialIndex < m_materials.size()) && (node->mNumMeshes > 1) )
	{
		tMesh.m_meshName += "_" + m_materials[mesh->mMaterialIndex].m_matName;
	}
	
	/* Loop over vertices. */
	for (int nv = 0; nv < mesh->mNumVertices; nv++)
	{
		/* Temporary vertex. */
		Vertex vert;
		/* Setting position, normal, texCoords from the mesh attributes. */
		vert.position = glm::vec3(mesh->mVertices[nv].x, mesh->mVertices[nv].y, mesh->mVertices[nv].z);
		vert.normal = glm::vec3(mesh->mNormals[nv].x, mesh->mNormals[nv].y, mesh->mNormals[nv].z);
		if (mesh->mTextureCoords[0])
		{
			vert.texCoords = glm::vec2(mesh->mTextureCoords[0][nv].x, mesh->mTextureCoords[0][nv].y);
		}
		else // fallback for texCoords
		{
			vert.texCoords = glm::vec2(0.0f);
		}
		/* Storing a copy of the temporary vertex in the temporary mesh vector of vertices. */
		tMesh.m_vertices.push_back(vert);
	}
	/* Loop over faces. */
	for (int nf = 0; nf < mesh->mNumFaces; nf++)
	{
		/* Temporary face. */
		aiFace face = mesh->mFaces[nf];
		/* Loop over indices of the face. */
		for (int ni = 0; ni < face.mNumIndices; ni++)		// mNumIndices should be 3 cause of aiProcessTriangulate flag (importer.ReadFile)
		{
			/* Storing indices in the temporary mesh vector of indices. */
			tMesh.m_indices.push_back(face.mIndices[ni]);
		}
	}
	/* Generates VAO, VBO, EBO for the mesh.*/
	tMesh.setup();

	return tMesh;
}

void Model::loadMaterial(const aiScene* scene)
{
	m_materials.clear();

	for (unsigned int nMat = 0; nMat < scene->mNumMaterials; nMat++) 
	{
		/* Temporary Material. */
		Material tMat;
		/* Get the material from the scene. */
		aiMaterial* aiMat = scene->mMaterials[nMat];
		
		/* Temporary aiColor3D and aiString.
		 * Needed to match the return type of the AI_MATKEYs. */
		aiColor3D tCol(0.f, 0.f, 0.f);
		aiString tName;

		/* Setting material ambient color. */
		if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, tCol) == AI_SUCCESS)
			tMat.setAmbient(glm::vec3(tCol.r, tCol.g, tCol.b));

		/* Setting material diffuse color. */
		if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, tCol) == AI_SUCCESS)
			tMat.setDiffuse(glm::vec3(tCol.r, tCol.g, tCol.b));

		/* Setting material specular color. */
		if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, tCol) == AI_SUCCESS)
			tMat.setSpecular(glm::vec3(tCol.r, tCol.g, tCol.b));

		/* Setting material emissive color. */
		if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, tCol) == AI_SUCCESS)
			tMat.setEmission(glm::vec3(tCol.r, tCol.g, tCol.b));

		/* Setting material shininess. */
		float tShin;
		if (aiMat->Get(AI_MATKEY_SHININESS, tShin) == AI_SUCCESS)
			tMat.setShininess(tShin);

		/* Setting material name.*/
		if (aiMat->Get(AI_MATKEY_NAME, tName) == AI_SUCCESS)
			tMat.m_matName = std::string(tName.C_Str());

		/* Storing the material. */
		m_materials.push_back(tMat);
		
	}
}

