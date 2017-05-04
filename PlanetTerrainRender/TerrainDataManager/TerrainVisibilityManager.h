#pragma once

#include "TerrainDataManager.h"
#include "vecmath.h"

#include <map>
#include <set>

class CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation
{
public:

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);
	//@}

private:

	void UpdateVisibilityRecursive(const CTerrainBlockData*, const vm::Vector3df& in_vPos);
	void AddVisibleBlock(const CTerrainBlockData*);
	bool IsFar(const CTerrainBlockData*, const vm::Vector3df& in_vPos) const;
	bool IsSomeChildVisible(const CTerrainBlockData*, const vm::Vector3df& in_vPos) const;

	std::map<C3DBaseObject*, const CTerrainBlockData*>	_mapTerrainBlockInfo;
	std::map<const CTerrainBlockData*, C3DBaseObject*>	_mapObjects;

	std::set<C3DBaseObject*> _setVisibleObjects;

	const CTerrainBlockData* _pRoot = nullptr;

	float	_fWorldScale = 1;
};