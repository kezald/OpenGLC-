#include "Mesh.h"
#include "TextureManager.h"
#include "LogManager.h"

#include "Debug.h"
#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

Mesh::MeshBuffers::MeshBuffers()
	: m_vaoID{ 0 }
	, m_positionsVBO{ 0 }
	, m_texCoordsVBO{ 0 }
	, m_normalsVBO{ 0 }
	, m_indicesEBO{ 0 }
{

}

Mesh::MeshBuffers::~MeshBuffers() {

}

Mesh::Mesh()
	: m_specularMapTextureID{ 0 }
	, m_diffuseMapTextureID{ 0 }
	, m_drawingMode{ 0 }
	, m_modelMatrix{ glm::mat4() }
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_scale{ 1.0f, 1.0f, 1.0f }
	, m_ambient{ 1.0f, 1.0f, 1.0f }
	, m_diffuse{ 1.0f, 1.0f, 1.0f }
	, m_specular{ 1.0f, 1.0f, 1.0f }
	, m_shininess(20.0f)
	, m_cubeMapTextureID{ 0 }
	, m_rotationXDeg{ 0.0f }
	, m_rotationYDeg{ 0.0f }
	, m_rotationZDeg{ 0.0f }
	, m_meshBuffers{ nullptr }
{

}

const glm::mat4& Mesh::GetModelMatrix() {
	m_modelMatrix = glm::mat4();
	m_modelMatrix = glm::translate(m_modelMatrix, m_position);
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationXDeg), glm::vec3(1.0f, 0.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationYDeg), glm::vec3(0.0f, 1.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationZDeg), glm::vec3(0.0f, 0.0f, 1.0f));
	m_modelMatrix = glm::scale(m_modelMatrix, m_scale);

	return m_modelMatrix;
}

void Mesh::Init(Mesh& mesh) {
	m_meshBuffers = mesh.m_meshBuffers;
}

void Mesh::Init(MeshDataHolder& meshDataHolder) {
	m_meshBuffers = std::make_shared<MeshBuffers>();

	glCheckError();
	// ============ Populate Buffers =============
	MeshData* data = meshDataHolder.GetMeshData();

	m_meshBuffers->m_vertexCount = (data->m_isAssimpLoaded) ?
		data->m_aiNumVertices :
		data->m_positions.size() / 3;
	m_drawingMode = data->m_drawingMode;
	m_meshBuffers->m_indexCount = data->m_indices.size();

	if (!data->m_positions.empty() || data->m_aiNumVertices > 0) {
		glGenBuffers(1, &m_meshBuffers->m_positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_positionsVBO);

		if (data->m_isAssimpLoaded) {
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(aiVector3D) * m_meshBuffers->m_vertexCount,
				data->m_aiPositions, GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER,
				data->m_positions.size() * sizeof(*data->m_positions.data()),
				data->m_positions.data(), GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glCheckError();

	if (!data->m_texCoords.empty()) {
		glGenBuffers(1, &m_meshBuffers->m_texCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_texCoordsVBO);

		glBufferData(GL_ARRAY_BUFFER,
			data->m_texCoords.size() * sizeof(*data->m_texCoords.data()),
			data->m_texCoords.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glCheckError();

	if (!data->m_normals.empty() || data->m_aiNumVertices > 0) {
		glGenBuffers(1, &m_meshBuffers->m_normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_normalsVBO);

		if (data->m_isAssimpLoaded) {
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(aiVector3D) * m_meshBuffers->m_vertexCount,
				data->m_aiNormals, GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER,
				data->m_normals.size() * sizeof(*data->m_normals.data()),
				data->m_normals.data(), GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glCheckError();

	if (!data->m_indices.empty()) {
		glGenBuffers(1, &m_meshBuffers->m_indicesEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshBuffers->m_indicesEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			data->m_indices.size() * sizeof(*data->m_indices.data()),
			data->m_indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glCheckError();

	// =============== Generate Vertex Array Object ===============
	if (m_meshBuffers->m_vaoID == 0) {
		glGenVertexArrays(1, &m_meshBuffers->m_vaoID);
	}

	if (m_meshBuffers->m_positionsVBO != 0) {
		glBindVertexArray(m_meshBuffers->m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_positionsVBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	glCheckError();

	if (m_meshBuffers->m_texCoordsVBO != 0) {
		glBindVertexArray(m_meshBuffers->m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_texCoordsVBO);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
	glCheckError();

	if (m_meshBuffers->m_normalsVBO != 0) {
		glBindVertexArray(m_meshBuffers->m_vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshBuffers->m_normalsVBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);
	}
	glCheckError();

	if (m_meshBuffers->m_indicesEBO != 0) {
		glBindVertexArray(m_meshBuffers->m_vaoID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshBuffers->m_indicesEBO);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glCheckError();
}

void Mesh::SetColor(glm::vec3 color) {
	m_ambient *= color;
	m_diffuse *= color;
	m_specular *= color;
}

void Mesh::SetTextureID(GLuint id) {
	m_specularMapTextureID = id;
}

void Mesh::SetDiffuseMapTextureID(GLuint id) {
	m_diffuseMapTextureID = id;
}

void Mesh::SetSpecularMapTextureID(GLuint id) {
	m_specularMapTextureID = id;
}

void Mesh::SetCubeMapTextureID(GLuint id) {
	m_cubeMapTextureID = id;
}

glm::vec3 Mesh::GetColor() {
	return m_ambient;
}

GLuint Mesh::GetDiffuseMapTextureID() {
	if (m_diffuseMapTextureID == 0) {
		return TextureManager::GetWhiteTextureID();
	}
	else {
		return m_diffuseMapTextureID;
	}
}

GLuint Mesh::GetSpecularMapTextureID() {
	if (m_specularMapTextureID == 0) {
		return TextureManager::GetWhiteTextureID();
	}
	else {
		return m_specularMapTextureID;
	}
}

GLuint Mesh::GetCubeMapTextureID() {
	return m_cubeMapTextureID;
}

bool Mesh::isDiffuseMapPresent() {
	return GetDiffuseMapTextureID() != TextureManager::GetWhiteTextureID();
}

bool Mesh::isSpecularMapPresent() {
	return GetSpecularMapTextureID() != TextureManager::GetWhiteTextureID();
}

bool Mesh::isCubeMapPresent() {
	return m_cubeMapTextureID != 0;
}