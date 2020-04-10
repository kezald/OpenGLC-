#include "MeshDataHolder.h"

#include <assert.h>

#include "Debug.h"
#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

MeshDataHolder::MeshDataHolder()
	: m_pMeshData{ nullptr }
{
	m_pMeshData = new MeshData();
}

MeshDataHolder::MeshDataHolder(MeshDataHolder&& holder) {
	delete m_pMeshData;
	m_pMeshData = holder.m_pMeshData;
	holder.m_pMeshData = nullptr;
}

MeshDataHolder& MeshDataHolder::operator=(MeshDataHolder&& holder) {
	delete m_pMeshData;
	m_pMeshData = holder.m_pMeshData;
	holder.m_pMeshData = nullptr;
	return *this;
}

MeshDataHolder::~MeshDataHolder()
{
	delete m_pMeshData;
	m_pMeshData = nullptr;
}

MeshData* MeshDataHolder::GetMeshData() {
	assert(m_pMeshData);
	return m_pMeshData;
}