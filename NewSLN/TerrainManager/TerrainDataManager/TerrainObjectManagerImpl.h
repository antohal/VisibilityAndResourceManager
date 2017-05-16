#pragma once

#include "TerrainObjectManager.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"


class CTerrainObjectManager::CTerrainObjectManagerImpl
{
public:

	void Init(const wchar_t* in_pcwszPlanetDirectory);

	CResourceManager* GetResourceManager();

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	void Update(float in_fDeltaTime);

	size_t GetVisibleTerrainObjectCount() const;

	size_t GetVisibleObjectID(size_t index) const;

private:

	CResourceManager*		_pResourceManager = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;
};