#pragma once

#include "Mesh.h"
#include "MeshDataHolder.h"
#include "TextureManager.h"

#include <assimp\scene.h>
#include <string>
#include <vector>

class Model
{
public:
	Model();
	~Model();

	void AddMesh(MeshDataHolder& holder);
	void LoadModel(Model& model);
	void LoadModel(std::string path);

	const glm::mat4& GetModelMatrix();

	static void SetTextureManager(TextureManager* pTextureManager);

public:
	std::vector<Mesh> m_meshes;
	glm::vec3 m_position;
	float m_rotationXDeg;
	float m_rotationYDeg;
	float m_rotationZDeg;
	glm::vec3 m_scale;
	glm::mat4 m_modelMatrix;
	bool m_isHighlight;
	bool m_isShadowCaster;
	bool m_isCenteredOnPlayer;

	bool m_isTransparentModel;
	bool m_isGetAlphaFromDiffuseMap;
	bool m_isReflectionMapped;
	bool m_isRefractionMapped;

private:
	static TextureManager* sm_pTextureManager;

	bool m_loaded;
	std::string m_directory;
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterialTextures(Mesh& mesh, aiMaterial* material, aiTextureType type);
};

