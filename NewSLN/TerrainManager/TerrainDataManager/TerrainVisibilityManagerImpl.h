#pragma once

#include "TerrainVisibilityManager.h"

class CTerrainVisibilityManager::CTerrainVisibilityManagerImpl
{
public:

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);
	//@}

private:

	void UpdateVisibilityRecursive(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos);
	void AddVisibleBlock(const CTerrainBlockDesc*);
	bool IsFar(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;
	bool IsSomeChildVisible(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;

	std::map<C3DBaseObject*, const CTerrainBlockDesc*>	_mapTerrainBlockInfo;
	std::map<const CTerrainBlockDesc*, C3DBaseObject*>	_mapObjects;

	std::set<C3DBaseObject*> _setVisibleObjects;

	const CTerrainBlockDesc* _pRoot = nullptr;

	float	_fWorldScale = 1;
};