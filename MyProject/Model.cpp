#include "Model.h"
#include "MeshDataHolder.h"

#include <iostream>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assert.h>
#include <glm\glm.hpp>

#include "Debug.h"
#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

TextureManager* Model::sm_pTextureManager = nullptr;

Model::Model()
	: m_loaded{ false }
	, m_modelMatrix{ glm::mat4() }
	, m_position{ 0.0f, 0.0f, 0.0f }
	, m_scale{ 1.0f, 1.0f, 1.0f }
	, m_rotationXDeg{ 0.0f }
	, m_rotationYDeg{ 0.0f }
	, m_rotationZDeg{ 0.0f }
	, m_isShadowCaster{ true }
	, m_isCenteredOnPlayer{ false }
	, m_isTransparentModel{ false }
	, m_isHighlight{ false }
	, m_isGetAlphaFromDiffuseMap{ true }
	, m_isReflectionMapped{ false }
	, m_isRefractionMapped{ false }
{
}


Model::~Model()
{
}

void Model::AddMesh(MeshDataHolder& holder) {
	m_meshes.emplace_back();
	Mesh& mesh = m_meshes.back();
	mesh.Init(holder);
}

void Model::LoadModel(Model& model) {
	for (Mesh& mesh : model.m_meshes) {
		m_meshes.emplace_back();
		m_meshes.back() = mesh;
	}

	m_loaded = true;
	m_directory = model.m_directory;
}

void Model::LoadModel(std::string path) {
	assert(!m_loaded);

	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate /*| aiProcess_FlipUVs*/);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::cout << importer.GetErrorString() << std::endl;
	}
	else {
		m_loaded = true;
	}

	m_directory = path.substr(0, path.find_last_of("/"));
	
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {

	for (int i = 0; i < node->mNumMeshes; i++) {
		unsigned int meshID = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[meshID];
		ProcessMesh(mesh, scene);
	}

	for (int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

void Model::ProcessMesh(aiMesh* aiMesh, const aiScene* scene) {
	MeshDataHolder holder;

	MeshData& data = *holder.GetMeshData();
	data.m_isAssimpLoaded = true;

	data.m_aiNumVertices = aiMesh->mNumVertices;
	data.m_aiPositions = aiMesh->mVertices;
	data.m_aiNormals = aiMesh->mNormals;

	if (aiMesh->GetNumUVChannels() > 1) {
		assert(false); //Interesting case - may ignore/comment out
	}

	if (aiMesh->mTextureCoords[0] && aiMesh->mNumUVComponents[0] == 2) {
		data.m_aiTexCoords = aiMesh->mTextureCoords[0];
	}

	for (int i = 0; i < aiMesh->mNumVertices; i++) {
		
		//data.m_positions.push_back(aiMesh->mVertices[i].x);
		//data.m_positions.push_back(aiMesh->mVertices[i].y);
		//data.m_positions.push_back(aiMesh->mVertices[i].z);

		//data.m_normals.push_back(aiMesh->mNormals[i].x);
		//data.m_normals.push_back(aiMesh->mNormals[i].y);
		//data.m_normals.push_back(aiMesh->mNormals[i].z);

		if (aiMesh->mTextureCoords[0] && aiMesh->mNumUVComponents[0] == 2) {
			data.m_texCoords.push_back(aiMesh->mTextureCoords[0][i].x);
			data.m_texCoords.push_back(aiMesh->mTextureCoords[0][i].y);
		}
	}

	for (int i = 0; i < aiMesh->mNumFaces; i++) {
		aiFace face = aiMesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			data.m_indices.push_back(face.mIndices[j]);
		}
	}
	data.m_drawingMode = GL_TRIANGLES;

	m_meshes.emplace_back();
	Mesh& mesh = m_meshes.back();
	mesh.Init(holder);

	if (aiMesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

		LoadMaterialTextures(mesh, material, aiTextureType::aiTextureType_DIFFUSE);
		LoadMaterialTextures(mesh, material, aiTextureType::aiTextureType_SPECULAR);

		aiColor3D color;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
			mesh.m_ambient = glm::vec3(color.r, color.g, color.b);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			mesh.m_diffuse = glm::vec3(color.r, color.g, color.b);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
			mesh.m_specular = glm::vec3(color.r, color.g, color.b);
		}

		float shininess;
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess) && shininess != 0.0f) {
			mesh.m_shininess = shininess;
		}

	}
}

void Model::LoadMaterialTextures(Mesh& mesh, aiMaterial* material, aiTextureType type) {
	assert(sm_pTextureManager);

	unsigned int count = material->GetTextureCount(type);
	assert(count < 2);

	for (int i = 0; i < count; i++) {
		aiString str;
		material->GetTexture(type, i, &str);

		if (type == aiTextureType::aiTextureType_DIFFUSE) {
			mesh.SetDiffuseMapTextureID(sm_pTextureManager->GetTexture(str.C_Str(), m_directory.c_str()));
		}
		else if (type == aiTextureType::aiTextureType_SPECULAR) {
			mesh.SetSpecularMapTextureID(sm_pTextureManager->GetTexture(str.C_Str(), m_directory.c_str()));
		}
		else {
			assert(false);
		}
	}
}

const glm::mat4& Model::GetModelMatrix() {
	m_modelMatrix = glm::mat4();
	m_modelMatrix = glm::translate(m_modelMatrix, m_position);
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationXDeg), glm::vec3(1.0f, 0.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationYDeg), glm::vec3(0.0f, 1.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotationZDeg), glm::vec3(0.0f, 0.0f, 1.0f));
	m_modelMatrix = glm::scale(m_modelMatrix, m_scale);

	return m_modelMatrix;
}

void Model::SetTextureManager(TextureManager* pTextureManager) {
	sm_pTextureManager = pTextureManager;
}