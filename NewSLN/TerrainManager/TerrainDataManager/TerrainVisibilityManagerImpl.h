#pragma once

#include "TerrainVisibilityManager.h"

#define MAX_LODS 20

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

	void CalculateLodDistances(float in_fCameraMeanFOV, const std::vector<size_t>& in_vecHFResolution, const std::vector<float>& in_vecLodDiameter, unsigned int in_uiMeanScreenResolution, float in_uiPixelsPerTexel);
	void SetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);
	void GetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);

private:

	void UpdateVisibleBlocks(const vm::Vector3df& in_vPos, unsigned int uiMaxDepth);

	void AddVisibleBlock(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos);

	enum class EUpdateVisibilityResult
	{
		INVISIBLE = 0,
		READY_AND_VISIBLE = 1,
		NOT_READY = 2,
	};

	EUpdateVisibilityResult UpdateVisibilityRecursive(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos);

	double GetDistance(const CTerrainBlockDesc* in_pTerrainBlock, const vm::Vector3df& in_vPos, double& out_Diameter);

	//bool IsBlockBehindEarth(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;


	unsigned int GetLodDepth(double dist) const;

	std::map<C3DBaseObject*, const CTerrainBlockDesc*>	_mapTerrainBlockInfo;
	std::map<const CTerrainBlockDesc*, C3DBaseObject*>	_mapObjects;

	std::set<C3DBaseObject*> _setVisibleObjects;

	const CTerrainBlockDesc* _pRoot = nullptr;

	double			_aLodDistances[MAX_LODS];

	float			_fWorldScale = 1;
	unsigned int	_uiMaxDepth = 0;

	//float			_fMaximumDistance = 2 * 6400000.f;
	//float			_fLodDistCoeff = 0.5f;
};
