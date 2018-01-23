#pragma once

#include <functional>
#include "vecmath.h"

#include <set>
#include <map>
#include <mutex>

#include "TerrainObjectManager.h"

#define MAX_LODS 20

class CTerrainVisibilityOwner
{
public:

	virtual bool			GetTerrainObjectProjection(TerrainObjectID ID, const vm::Vector3df& in_pvPosFrom, vm::Vector3df& out_pvProjection, vm::Vector3df& out_vNormal) const = 0;
	virtual bool			GetTerrainObjectClosestPoint(TerrainObjectID ID, const vm::Vector3df& in_pvPosFrom, vm::Vector3df& out_pvClosestPoint, vm::Vector3df& out_pvNormal) const = 0;
	virtual vm::Vector3df	GetTerrainObjectCenter(TerrainObjectID ID) const = 0;
	virtual double			GetTerrainObjectDiameter(TerrainObjectID ID) const = 0;
	virtual bool			IsDrawingParentIfNotReadyChilds() const = 0;
	virtual bool			IsAllObjectsReady(const std::vector<TerrainObjectID>& vecObjs) const = 0;
	virtual bool			IsDataReady(TerrainObjectID ID) const = 0;
};

class CTerrainObjectVisibleSubtree
{
public:

	CTerrainObjectVisibleSubtree(CTerrainObjectManager* in_pObjectManager) : _pObjectManager(in_pObjectManager) {}

	void setToObjects(const std::set<TerrainObjectID>& setObjects) {
		_setObjects = setObjects;
	}

	const std::set<TerrainObjectID>&	objects() const {
		return _setObjects;
	}

	bool hasObject(TerrainObjectID ID) const {
		return _setObjects.find(ID) != _setObjects.end();
	}

	void setLastMaxDepth(unsigned int in_uiLastMaxDepth) {
		_uiLastMaxDepth = in_uiLastMaxDepth;
	}

	void update(const std::set<TerrainObjectID>& setVisObjects, const std::set<TerrainObjectID>& setDataReadyObjects);

private:

	bool getReadyAndVisibleChildrenRecursive(TerrainObjectID ID, std::vector<TerrainObjectID>& out_vecReadyAndVisible, const std::set<TerrainObjectID>& setVisObjects, const std::set<TerrainObjectID>& setDataReadyObjects) const;

	std::set<TerrainObjectID>	_setObjects;
	unsigned int				_uiLastMaxDepth = 0;

	CTerrainObjectManager*		_pObjectManager = nullptr;
};

class CTerrainVisibility
{
public:

	CTerrainVisibility(CTerrainObjectManager*, CTerrainVisibilityOwner*, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth);

	void	UpdateObjectsVisibility(float in_fDeltaTime, const vm::Vector3df& in_vPos);
	
	void	SetLastLODDistanceOnSurface(double in_dfDistM) {
		_dfLastLODDistanceOnEarth = in_dfDistM;
	}

	void	CalculateLodDistances(float in_fCameraMeanFOV, const std::vector<size_t>& in_vecHFResolution, const std::vector<float>& in_vecLodDiameter, unsigned int in_uiMeanScreenResolution, float in_uiPixelsPerTexel);
	void	SetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);
	void	GetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);


	const std::set<TerrainObjectID>&	GetVisibleObjects() const {
		return _setVisibleObjects;
	}

	unsigned int GetLastMaxDepth() const {
		return _uiLastMaxDepth;
	}

private:

	double			GetDistance(TerrainObjectID ID, const vm::Vector3df& in_vPos, double& out_Diameter);
	unsigned int	GetLodDepth(double dist) const;

	void			UpdateVisibleBlocks(const vm::Vector3df& in_vPos, unsigned int uiMaxDepth);
	void			AddVisibleBlock(TerrainObjectID ID);
	void			CalculateLodDistanceCoeff(double height);

	enum class EUpdateVisibilityResult
	{
		INVISIBLE = 0,
		READY_AND_VISIBLE = 1,
		NOT_READY = 2,
	};

	EUpdateVisibilityResult UpdateVisibilityRecursive(TerrainObjectID ID, const vm::Vector3df& in_vPos,
		const std::function<void(TerrainObjectID)>& in_AddVisObjFunc,
		const std::function<bool(TerrainObjectID)>* in_pAdditionalCheckFunc, bool* out_bSubtreeDataReady = nullptr);

	
	CTerrainVisibilityOwner*				_owner = nullptr;
	CTerrainObjectManager*					_objectManager = nullptr;

	double									_aLodDistances[MAX_LODS];

	float									_fWorldScale = 1;
	unsigned int							_uiMaxDepth = 0;

	double									_dfLastLODDistanceOnEarth = -1;
	double									_dfDistancesCoeff = 1.f;

	unsigned int							_uiLastMaxDepth = 0;
	vm::Vector3df							_vLastPos = vm::Vector3df(0, 0, 0);

	std::set<TerrainObjectID>				_setVisibleObjects;

	std::vector<TerrainObjectID>			_vecRootObjects;
};
