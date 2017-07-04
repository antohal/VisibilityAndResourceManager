#pragma once

#include "TerrainVisibilityManager.h"

class CTerrainVisibilityManager::CTerrainVisibilityManagerImpl
{
public:

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);

	unsigned int GetVisibleObjectsCount() const;
	C3DBaseObject* GetVisibleObject(unsigned int i);

	//@}

private:

	void AddVisibleBlock(const CTerrainBlockDesc*);
	bool UpdateVisibilityRecursive(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos);

	double GetDistance(const CTerrainBlockDesc* in_pTerrainBlock, const vm::Vector3df& in_vPos, double& out_Diameter);

	bool IsBlockBehindEarth(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;


	unsigned int GetLodDepth(double dist) const;

	std::map<C3DBaseObject*, const CTerrainBlockDesc*>	_mapTerrainBlockInfo;
	std::map<const CTerrainBlockDesc*, C3DBaseObject*>	_mapObjects;

	std::set<C3DBaseObject*> _setVisibleObjects;

	const CTerrainBlockDesc* _pRoot = nullptr;

	float			_fWorldScale = 1;
	unsigned int	_uiMaxDepth = 0;

	float			_fMaximumDistance = 2 * 6400000.f;
	float			_fLodDistCoeff = 0.5f;
};