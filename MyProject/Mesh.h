#pragma once
#include "Vertex3D.h"
#include "MeshDataHolder.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL\glew.h>
#include <vector>
#include <assimp\scene.h>
#include <memory>

class Mesh
{
	class MeshBuffers {
	public:
		MeshBuffers();
		~MeshBuffers();

	public:
		GLuint m_vaoID;
		GLuint m_positionsVBO;
		GLuint m_texCoordsVBO;
		GLuint m_normalsVBO;
		GLuint m_indicesEBO;

		GLuint m_vertexCount;
		GLuint m_indexCount;
	};

public:
	Mesh();

	void Init(Mesh& mesh);
	void Init(MeshDataHolder& meshDataHolder);

	const glm::mat4& GetModelMatrix();
	glm::vec3 GetColor();
	GLuint GetSpecularMapTextureID();
	GLuint GetDiffuseMapTextureID();
	GLuint GetCubeMapTextureID();
	bool isDiffuseMapPresent();
	bool isSpecularMapPresent();
	bool isCubeMapPresent();

	void SetColor(glm::vec3 color);
	void SetTextureID(GLuint id);
	void SetDiffuseMapTextureID(GLuint id);
	void SetSpecularMapTextureID(GLuint id);
	void SetCubeMapTextureID(GLuint id);

public:
	std::shared_ptr<MeshBuffers> m_meshBuffers;

	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	float m_shininess;
	GLenum m_drawingMode;

	float m_rotationXDeg;
	float m_rotationYDeg;
	float m_rotationZDeg;

private:
	glm::mat4 m_modelMatrix;
	GLuint m_specularMapTextureID;
	GLuint m_diffuseMapTextureID;
	GLuint m_cubeMapTextureID;
};

