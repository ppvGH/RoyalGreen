#include "model.h"
#include <iostream>

void Model::loadModel(const std::string& filepath)
{
	/* Importer reads the model file into a scene. */
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipUVs);

	/* Error checker. */
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR ASSIMP: " << importer.GetErrorString() << std::endl;
		return;
	}


	loadMaterial(scene);

	processNode(scene->mRootNode, scene);

}


void Model::draw(Shader& shader) const
{
	for (const auto& mesh : m_meshes)
	{
		if (mesh.m_matIndex > -1 && mesh.m_matIndex < m_materials.size())
		{
			m_materials[mesh.m_matIndex].apply(shader);
		}
		mesh.draw();
	}
}




void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (int nm = 0; nm < node->mNumMeshes; nm++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[nm]];
		m_meshes.push_back(processMesh(mesh));
	}

	for (int nc = 0; nc < node->mNumChildren; nc++)
	{
		processNode(node->mChildren[nc], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh)
{
	/* Temporary mesh object. */
	Mesh tMesh;
	/* Setup the material index of the temporary mesh with. */
	tMesh.m_matIndex = mesh->mMaterialIndex;

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
			tMat.m_ambient = glm::vec3(tCol.r, tCol.g, tCol.b);

		/* Setting material diffuse color. */
		if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, tCol) == AI_SUCCESS)
			tMat.m_diffuse = glm::vec3(tCol.r, tCol.g, tCol.b);

		/* Setting material specular color. */
		if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, tCol) == AI_SUCCESS)
			tMat.m_specular = glm::vec3(tCol.r, tCol.g, tCol.b);

		/* Setting material emissive color. */
		if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, tCol) == AI_SUCCESS)
			tMat.m_emission = glm::vec3(tCol.r, tCol.g, tCol.b);

		/* Setting material shininess. */
		float tShin;
		if (aiMat->Get(AI_MATKEY_SHININESS, tShin) == AI_SUCCESS)
			tMat.m_shininess = tShin;

		/* Setting material name.*/
		if (aiMat->Get(AI_MATKEY_NAME, tName) == AI_SUCCESS)
			tMat.m_matName = std::string(tName.C_Str());

		/* Storing the material. */
		m_materials.push_back(tMat);
	}
}

