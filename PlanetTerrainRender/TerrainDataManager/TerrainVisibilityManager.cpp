#include "TerrainVisibilityManager.h"

CTerrainVisibilityManager::CTerrainVisibilityManager()
{
	_implementation = new CTerrainVisibilityManagerImplementation();
}

CTerrainVisibilityManager::~CTerrainVisibilityManager()
{
	delete _implementation;
}

void CTerrainVisibilityManager::Init(C3DBaseTerrainObjectManager* in_pMeshTree)
{
	_implementation->Init(in_pMeshTree);
}

bool CTerrainVisibilityManager::IsObjectVisible(C3DBaseObject* in_pObject) const
{
	return _implementation->IsObjectVisible(in_pObject);
}

void CTerrainVisibilityManager::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_implementation->UpdateObjectsVisibility(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::Init(C3DBaseTerrainObjectManager * in_pMeshTree)
{
}

bool CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::IsObjectVisible(C3DBaseObject * in_pObject) const
{
	return false;
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
}
