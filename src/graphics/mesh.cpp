#include "mesh.h"
#include "../core/validation.h"
#include <utility>
#include <limits>
#include <cstddef>
#include <type_traits>

// GPU attribute offsets require a standard-layout vertex on every compiler.
static_assert(std::is_standard_layout_v<Vertex>, "Vertex layout must support offsetof");
#include <glad/glad.h>

Mesh::Mesh() :
	m_VAO(0),
	m_VBO(0),
	m_EBO(0),
	m_matIndex(-1),
	m_UVresize(0),
	m_mixTex(true),
    m_shader(nullptr)
{
}


void Mesh::setup()
{
	if(m_VAO || m_VBO || m_EBO)throw std::logic_error("Mesh is already uploaded");
    validation::require(!m_vertices.empty() && !m_indices.empty(),"Cannot upload empty mesh");
	// Generate the Vertex Array Object.
	glGenVertexArrays(1, &m_VAO);
	// Generate the Vertex Buffer Object.
	glGenBuffers(1, &m_VBO);
	// Generate the Element Array Object.
	glGenBuffers(1, &m_EBO);

	// Bind the VAO.
	glBindVertexArray(m_VAO);

	// Bind the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	// Buffer an array of vertex data into the VBO; 
	// vec.data() points to the first element of vec, and if it's an empty vec it becomes a nullptr.
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW); 

	// Bind the EBO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	// Buffer an array of vertex indices (elements) into the EBO.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	// Layout location 0: position.
	// Pointer is at the beginning of the data.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Layout location 1: normal.
	// Pointer position is computed by offsetof macro.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(1);

	// Layout location 2: texCoords.
	// Pointer position is computed by offsetof macro.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoords));
	glEnableVertexAttribArray(2);

	// Unbind the Vertex Array.
	glBindVertexArray(0);

}

void Mesh::draw() const
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0); // not really necessary
}

glm::vec3 Mesh::getCenter() const
{
	validation::require(!m_vertices.empty(),"Cannot compute center of empty mesh");
	/* Average of vertices positions. */
	glm::vec3 center = glm::vec3(0.0f);
	for (const auto& vert : m_vertices) center += vert.getPosition();
	center /= m_vertices.size();

	return center;
}

glm::vec3 Mesh::getGlobalNormal() const
{
	/* Normalized sum of the vertices normals. */
	glm::vec3 gNormal = glm::vec3(0.0f);
	for (const auto& vert : m_vertices)	gNormal += vert.getNormal();
	validation::require(validation::finite(gNormal) && glm::length(gNormal)>0,"Mesh normals have no finite nonzero sum");
	gNormal = glm::normalize(gNormal);

	return gNormal;
}


bool Mesh::intersectRayTriangle(const Ray& localRay, float& tOut) const
{
	bool hit = false;
	float tMin = std::numeric_limits<float>::max();

	for (const auto& triData : m_triangleData)
	{
		float t;
		if (mollerTrumbore(localRay, triData, t))
		{
			if (t < tMin)
			{
				tMin = t;
				hit = true;
			}
		}
	}

	if (hit) tOut = tMin;
	return hit;
}

void Mesh::release() noexcept { if(m_VAO)glDeleteVertexArrays(1,&m_VAO);if(m_VBO)glDeleteBuffers(1,&m_VBO);if(m_EBO)glDeleteBuffers(1,&m_EBO);m_VAO=m_VBO=m_EBO=0; }
Mesh::~Mesh() { release(); }
Mesh::Mesh(Mesh&& other) noexcept:Mesh() { *this=std::move(other); }
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if(this==&other)return *this;
    release();m_meshName=std::move(other.m_meshName);m_vertices=std::move(other.m_vertices);
    m_indices=std::move(other.m_indices);m_triangleData=std::move(other.m_triangleData);
    m_matIndex=other.m_matIndex;m_UVresize=other.m_UVresize;m_mixTex=other.m_mixTex;m_shader=other.m_shader;
    m_VAO=std::exchange(other.m_VAO,0);m_VBO=std::exchange(other.m_VBO,0);m_EBO=std::exchange(other.m_EBO,0);return *this;
}

void Mesh::rebuildTriangleCache() {
    validation::require(m_indices.size()%3==0,"Triangle indices must occur in triples");
    std::vector<TriangleData> cache;
    for(std::size_t i=0;i<m_indices.size();i+=3) {
        auto a=m_indices[i],b=m_indices[i+1],c=m_indices[i+2];
        const auto& A=m_vertices.at(a).getPosition();const auto& B=m_vertices.at(b).getPosition();const auto& C=m_vertices.at(c).getPosition();
        auto normal=glm::cross(B-A,C-A);
        validation::require(validation::finite(A) && validation::finite(B) && validation::finite(C) && validation::finite(normal),"Imported triangle positions must be finite");
        // Blender exports include zero-area faces. Keep the draw indices, but
        // omit unpickable degenerate faces from the intersection cache.
        if(glm::length(normal)==0)continue;
        cache.emplace_back(a,b,c,m_vertices);
    }
    m_triangleData=std::move(cache);
}
