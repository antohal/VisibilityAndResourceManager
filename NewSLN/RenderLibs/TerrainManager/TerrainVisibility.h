#pragma once

#include <functional>
#include "vecmath.h"

#include <set>
#include <map>

#include "TerrainObjectManager.h"

#define MAX_LODS 20

class CTerrainVisibility
{
public:

	CTerrainVisibility(CTerrainObjectManager*, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth);

	void	SetNewObjectHandler(const std::function<void(TerrainObjectID)>&);
	void	SetDeleteObjectHandler(const std::function<void(TerrainObjectID)>&);

	void	UpdateObjectsVisibility(float in_fDeltaTime, const vm::Vector3df& in_vPos);
	void	SetMaxInvisibleTime(float in_fMaxInvisibleTime) {
		_fMaxInvisibleTime = in_fMaxInvisibleTime;
	}


	void	CalculateLodDistances(float in_fCameraMeanFOV, const std::vector<size_t>& in_vecHFResolution, const std::vector<float>& in_vecLodDiameter, unsigned int in_uiMeanScreenResolution, float in_uiPixelsPerTexel);
	void	SetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);
	void	GetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods);


	const std::set<TerrainObjectID>&	GetVisibleObjects() const {
		return _setVisibleObjects;
	}

	void	RequestForAlive(TerrainObjectID ID);

private:

	double			GetDistance(TerrainObjectID ID, const vm::Vector3df& in_vPos, double& out_Diameter);
	unsigned int	GetLodDepth(double dist) const;
	double			AngularDistance(double a1, double a2);

	void			UpdateVisibleBlocks(const vm::Vector3df& in_vPos, unsigned int uiMaxDepth);
	void			AddVisibleBlock(TerrainObjectID ID);

	void			UpdateObjectsLifetime(float in_fDeltaTime);

	enum class EUpdateVisibilityResult
	{
		INVISIBLE = 0,
		READY_AND_VISIBLE = 1,
		NOT_READY = 2,
	};

	EUpdateVisibilityResult UpdateVisibilityRecursive(TerrainObjectID ID, const vm::Vector3df& in_vPos);


	struct SAliveObject
	{
		SAliveObject() {};
		SAliveObject(TerrainObjectID id) : ID(id) {}

		TerrainObjectID ID;
		float			timeBeingInvisible = 0;
	};


	CTerrainObjectManager*					_objectManager = nullptr;

	std::function<void(TerrainObjectID)>	_newObjectHandler;
	std::function<void(TerrainObjectID)>	_deleteObjectHandler;

	double									_aLodDistances[MAX_LODS];

	float									_fWorldScale = 1;
	unsigned int							_uiMaxDepth = 0;

	float									_fMaxInvisibleTime = 5.f;

	std::set<TerrainObjectID>				_setVisibleObjects;
	std::map<TerrainObjectID, SAliveObject> _mapAliveObjects;

	std::vector<TerrainObjectID>			_vecRootObjects;
};
