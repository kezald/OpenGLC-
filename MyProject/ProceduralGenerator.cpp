#include "ProceduralGenerator.h"
#include "Constants.h"
#include "Vertex3D.h"

#define _USE_MATH_DEFINES
#include <math.h>

MeshDataHolder ProceduralGenerator::GenerateTriangleModel() {
	MeshData::VertPositionDataType positions[] = {
		-1,-1,0,
		 1,-1,0,
		 0,1,0
	};
	MeshData::VertTexCoordDataType texCoords[] = {
		 0.0, 0.0,
		 1.0, 0.0,
		 0.5, 1.0
	};
	MeshData::VertNormalDataType normals[] = {
		 0.0, 0.0, 1.0f,
		 0.0, 0.0, 1.0f,
		 0.0, 0.0, 1.0f
	};

	MeshDataHolder holder;
	for (int i = 0; i < (sizeof(positions) / sizeof(MeshData::VertPositionDataType)); i++) {
		holder.GetMeshData()->m_positions.push_back(positions[i]);
	}
	for (int i = 0; i < (sizeof(normals) / sizeof(MeshData::VertNormalDataType)); i++) {
		holder.GetMeshData()->m_normals.push_back(normals[i]);
	}
	for (int i = 0; i < (sizeof(texCoords) / sizeof(MeshData::VertTexCoordDataType)); i++) {
		holder.GetMeshData()->m_texCoords.push_back(texCoords[i]);
	}

	holder.GetMeshData()->m_drawingMode = GL_TRIANGLES;

	return holder;
}

MeshDataHolder ProceduralGenerator::GenerateCubeModel() {
	MeshData::VertPositionDataType positions[] = {
		// Back face
   -0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f,  0.5f, -0.5f,
   // Front face
   -0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   -0.5f, -0.5f,  0.5f,
   // Left face
   -0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   // Right face
	0.5f,  0.5f,  0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	// Bottom face
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	// Top face
	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	};

	MeshData::VertNormalDataType normals[] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	MeshData::VertTexCoordDataType texCoords[] = {
	0.0f, 0.0f, // Bottom-left
	1.0f, 1.0f, // top-right
	1.0f, 0.0f, // bottom-right         
	1.0f, 1.0f, // top-right
	0.0f, 0.0f, // bottom-left
	0.0f, 1.0f, // top-left

	0.0f, 0.0f, // bottom-left
	1.0f, 0.0f, // bottom-right
	1.0f, 1.0f, // top-right
	1.0f, 1.0f, // top-right
	0.0f, 1.0f, // top-left
	0.0f, 0.0f, // bottom-left

	1.0f, 0.0f, // top-right
	1.0f, 1.0f, // top-left
	0.0f, 1.0f, // bottom-left
	0.0f, 1.0f, // bottom-left
	0.0f, 0.0f, // bottom-right
	1.0f, 0.0f, // top-right

	1.0f, 0.0f, // top-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right         
	0.0f, 1.0f, // bottom-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f, // bottom-left     

	0.0f, 1.0f, // top-right
	1.0f, 1.0f, // top-left
	1.0f, 0.0f, // bottom-left
	1.0f, 0.0f, // bottom-left
	0.0f, 0.0f, // bottom-right
	0.0f, 1.0f, // top-right

	0.0f, 1.0f, // top-left
	1.0f, 0.0f, // bottom-right
	1.0f, 1.0f, // top-right     
	1.0f, 0.0f, // bottom-right
	0.0f, 1.0f, // top-left
	0.0f, 0.0f  // bottom-left     
	};

	MeshDataHolder holder;
	for (int i = 0; i < (sizeof(positions) / sizeof(MeshData::VertPositionDataType)); i++) {
		holder.GetMeshData()->m_positions.push_back(positions[i]);
	}
	for (int i = 0; i < (sizeof(normals) / sizeof(MeshData::VertNormalDataType)); i++) {
		holder.GetMeshData()->m_normals.push_back(normals[i]);
	}
	for (int i = 0; i < (sizeof(texCoords) / sizeof(MeshData::VertTexCoordDataType)); i++) {
		holder.GetMeshData()->m_texCoords.push_back(texCoords[i]);
	}

	holder.GetMeshData()->m_drawingMode = GL_TRIANGLES;

	return holder;
}

MeshDataHolder ProceduralGenerator::GenerateSphereModel() {
	MeshDataHolder holder;

	float sphereRadius = 1.0f;
	int slices = 40;
	int stacks = 40;

	float yStart = sphereRadius;
	float yEnd = -sphereRadius;
	float xStart = sphereRadius;
	float xEnd = -sphereRadius;

	std::vector<std::vector<glm::vec3>> stackVertices;
	stackVertices.resize(stacks - 1);

	for (int i = 1; i < stacks; i++) {
		float y = yStart - (i / (float)stacks) * (yStart - yEnd);
		float cirCurRadius = std::sqrt(sphereRadius * sphereRadius - y * y);

		for (int j = 0; j <= slices; j++) {
			float curAngle = (j / (float)slices) * 2.0f * M_PI;
			float x = cirCurRadius * std::cos(curAngle);
			float z = cirCurRadius * std::sin(curAngle);
			stackVertices[i - 1].push_back(glm::vec3(x, y, -z)); //Invert z from OpenGL representation into cosine coordinate system
		}
	}

	for (int j = 1; j < stackVertices[0].size(); j++) {
		holder.GetMeshData()->AddPosition(stackVertices[0][j]);
		holder.GetMeshData()->AddNormal(stackVertices[0][j]);
		holder.GetMeshData()->AddPosition(glm::vec3(0.0f, yStart, 0.0f));
		holder.GetMeshData()->AddNormal(glm::vec3(0.0f, yStart, 0.0f));
		holder.GetMeshData()->AddPosition(stackVertices[0][j - 1]);
		holder.GetMeshData()->AddNormal(stackVertices[0][j - 1]);
	}

	for (int i = 1; i < stackVertices.size(); i++) {
		for (int j = 1; j < stackVertices[0].size(); j++) {
			holder.GetMeshData()->AddPosition(stackVertices[i][j - 1]);
			holder.GetMeshData()->AddNormal(stackVertices[i][j - 1]);
			holder.GetMeshData()->AddPosition(stackVertices[i][j]);
			holder.GetMeshData()->AddNormal(stackVertices[i][j]);
			holder.GetMeshData()->AddPosition(stackVertices[i - 1][j]);
			holder.GetMeshData()->AddNormal(stackVertices[i - 1][j]);

			holder.GetMeshData()->AddPosition(stackVertices[i][j - 1]);
			holder.GetMeshData()->AddNormal(stackVertices[i][j - 1]);
			holder.GetMeshData()->AddPosition(stackVertices[i - 1][j]);
			holder.GetMeshData()->AddNormal(stackVertices[i - 1][j]);
			holder.GetMeshData()->AddPosition(stackVertices[i - 1][j - 1]);
			holder.GetMeshData()->AddNormal(stackVertices[i - 1][j - 1]);
		}
	}

	for (int j = 1; j < stackVertices[stackVertices.size() - 1].size(); j++) {
		holder.GetMeshData()->AddPosition(glm::vec3(0.0f, yEnd, 0.0f));
		holder.GetMeshData()->AddNormal(glm::vec3(0.0f, yEnd, 0.0f));
		holder.GetMeshData()->AddPosition(stackVertices[stackVertices.size() - 1][j]);
		holder.GetMeshData()->AddNormal(stackVertices[stackVertices.size() - 1][j]);
		holder.GetMeshData()->AddPosition(stackVertices[stackVertices.size() - 1][j - 1]);
		holder.GetMeshData()->AddNormal(stackVertices[stackVertices.size() - 1][j - 1]);
	}

	holder.GetMeshData()->m_drawingMode = GL_TRIANGLES;

	return holder;
}

MeshDataHolder ProceduralGenerator::GenerateGridModel() {
	//Extends from -xLength/2.0 to xLength/2.0f, similarly with zLength
	double xLength = 2;
	double zLength = 2;
	static const int xPieces = 250;
	static const int zPieces = 250;
	int xPiecesPerTexture = 1;
	int zPiecesPerTexture = 1;
	static const int xVertNum = xPieces + 1;
	static const int zVertNum = zPieces + 1;

	MeshDataHolder holder;
	holder.GetMeshData()->m_positions.reserve(xVertNum * zVertNum * 3);
	holder.GetMeshData()->m_texCoords.reserve(xVertNum * zVertNum * 2);

	for (int i = 0; i < xVertNum; i++) {
		double x = -xLength / 2.0 + (i / (double)(xVertNum - 1)) * xLength;

		for (int j = 0; j < zVertNum; j++) {
			double z = -zLength / 2.0 + (j / (double)(zVertNum - 1)) * zLength;

			holder.GetMeshData()->m_positions.push_back(x);
			holder.GetMeshData()->m_positions.push_back(0);
			holder.GetMeshData()->m_positions.push_back(z);

			holder.GetMeshData()->m_texCoords.push_back((i) / ((float)xPiecesPerTexture));
			holder.GetMeshData()->m_texCoords.push_back((j) / ((float)zPiecesPerTexture));
		}
	}

	holder.GetMeshData()->m_indices.reserve((xVertNum - 1)*(zVertNum) * 2 + (xVertNum - 1));
	for (int i = 0; i < xVertNum - 1; i++) {
		for (int j = 0; j < zVertNum; j++) {
			int curVertIndex = i * zVertNum + j;
			holder.GetMeshData()->m_indices.push_back(curVertIndex);
			holder.GetMeshData()->m_indices.push_back(curVertIndex + zVertNum);
		}

		if (i + 1 < xVertNum - 1) {
			holder.GetMeshData()->m_indices.push_back(Constants::PRIMITIVE_RESTART_INDEX);
		}
	}

	for (int i = 0; i < xVertNum * zVertNum; i++) {
		holder.GetMeshData()->m_normals.push_back(0.0f);
		holder.GetMeshData()->m_normals.push_back(1.0f);
		holder.GetMeshData()->m_normals.push_back(0.0f);
	}

	holder.GetMeshData()->m_drawingMode = GL_TRIANGLE_STRIP;

	return holder;
}

MeshDataHolder ProceduralGenerator::Generate2DScreenQuad() {
	MeshData::VertPositionDataType positions[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};


	MeshData::VertTexCoordDataType texCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	MeshData::VertNormalDataType normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	//MeshData::VertPositionDataType positions[] = {
	//	-1.0f, 0.0f, 1.0f,
	//	-1.0f, 0.0f, -1.0f,
	//	1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f,
	//	-1.0f, 0.0f, -1.0f,
	//	1.0f, 0.0f, -1.0f
	//};


	//MeshData::VertTexCoordDataType texCoords[] = {
	//	0.0f, 1.0f,
	//	0.0f, 0.0f,
	//	1.0f, 1.0f,
	//	1.0f, 1.0f,
	//	0.0f, 0.0f,
	//	1.0f, 0.0f
	//};

	//MeshData::VertNormalDataType normals[] = {
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f
	//};

	MeshDataHolder holder;
	for (int i = 0; i < (sizeof(positions) / sizeof(MeshData::VertPositionDataType)); i++) {
		holder.GetMeshData()->m_positions.push_back(positions[i]);
	}
	for (int i = 0; i < (sizeof(normals) / sizeof(MeshData::VertNormalDataType)); i++) {
		holder.GetMeshData()->m_normals.push_back(normals[i]);
	}
	for (int i = 0; i < (sizeof(texCoords) / sizeof(MeshData::VertTexCoordDataType)); i++) {
		holder.GetMeshData()->m_texCoords.push_back(texCoords[i]);
	}
	holder.GetMeshData()->m_drawingMode = GL_TRIANGLES;
	return holder;
}