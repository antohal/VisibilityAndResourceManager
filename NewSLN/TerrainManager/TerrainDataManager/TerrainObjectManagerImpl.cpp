#include "TerrainObjectManagerImpl.h"

CTerrainObjectManager::CTerrainObjectManager()
{
	_implementation = new CTerrainObjectManagerImpl;
}

CTerrainObjectManager::~CTerrainObjectManager()
{
	delete _implementation;
}

void CTerrainObjectManager::Init(CTerrainObjectCreator* in_pObjectCreator, const wchar_t* in_pcwszPlanetDirectory)
{
	_implementation->Init(in_pObjectCreator, in_pcwszPlanetDirectory);
}

CResourceManager* CTerrainObjectManager::GetResourceManager()
{
	return _implementation->GetResourceManager();
}

void CTerrainObjectManager::SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection)
{
	_implementation->SetViewProjection(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

void CTerrainObjectManager::Update(float in_fDeltaTime)
{
	_implementation->Update(in_fDeltaTime);
}

size_t CTerrainObjectManager::GetVisibleTerrainObjectCount() const
{
	return _implementation->GetVisibleTerrainObjectCount();
}

size_t CTerrainObjectManager::GetVisibleObjectID(size_t index) const
{
	return _implementation->GetVisibleObjectID(index);
}


//
// CTerrainObjectManager::CTerrainObjectManagerImpl
//

void CTerrainObjectManager::CTerrainObjectManagerImpl::Init(CTerrainObjectCreator* in_pObjectCreator, const wchar_t* in_pcwszPlanetDirectory)
{

}

CResourceManager* CTerrainObjectManager::CTerrainObjectManagerImpl::GetResourceManager()
{
	return nullptr;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection)
{

}

void CTerrainObjectManager::CTerrainObjectManagerImpl::Update(float in_fDeltaTime)
{

}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleTerrainObjectCount() const
{
	return 0;
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleObjectID(size_t index) const
{
	return 0;
}
