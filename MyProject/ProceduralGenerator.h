#pragma once

#include "MeshDataHolder.h"

class ProceduralGenerator
{
public:
	static MeshDataHolder GenerateTriangleModel();
	static MeshDataHolder GenerateCubeModel();
	static MeshDataHolder GenerateSphereModel();
	static MeshDataHolder GenerateGridModel();
	static MeshDataHolder Generate2DScreenQuad();
};

