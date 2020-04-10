#pragma once

#include <GL\glew.h>
#include <vector>
#include <glm\glm.hpp>
#include <assimp\scene.h>

struct MeshData
{
	using VertPositionDataType = GLfloat;
	using VertColorDataType = GLfloat;
	using VertTexCoordDataType = GLfloat;
	using VertNormalDataType = GLfloat;
	using VertIndicesType = GLuint;

	// ======== ASSIMP data =========
	bool m_isAssimpLoaded = false;
	aiVector3D* m_aiPositions;
	aiVector3D* m_aiNormals;
	aiVector3D* m_aiTexCoords;
	unsigned int m_aiNumVertices = 0;

	// ======== Ordinary Data =======
	std::vector<VertPositionDataType> m_positions;
	std::vector<VertTexCoordDataType> m_texCoords;
	std::vector<VertNormalDataType> m_normals;
	std::vector<VertIndicesType> m_indices;

	GLenum m_drawingMode;

	void AddPosition(glm::vec3 pos) {
		m_positions.push_back(pos[0]);
		m_positions.push_back(pos[1]);
		m_positions.push_back(pos[2]);
	}

	void AddNormal(glm::vec3 normal) {
		m_normals.push_back(normal[0]);
		m_normals.push_back(normal[1]);
		m_normals.push_back(normal[2]);
	}
};


class MeshDataHolder
{
public:
	MeshDataHolder();
	~MeshDataHolder();
	MeshDataHolder(const MeshDataHolder& holder) = delete;
	MeshDataHolder(MeshDataHolder&& holder);
	MeshDataHolder& operator=(const MeshDataHolder& holder) = delete;
	MeshDataHolder& operator=(MeshDataHolder&& holder);

	MeshData* GetMeshData();

private:
	MeshData * m_pMeshData = nullptr;
};

