#pragma once

#include "TerrainDataManager.h"

#include <map>

class CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation
{
public:

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);
	//@}

private:

	std::map<C3DBaseObject*, const CTerrainBlockData*>	_mapTerrainBlockInfo;

};