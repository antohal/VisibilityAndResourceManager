#include "TerrainVisibility.h"

#include "Log.h"
#include "wgs84.h"

#include <algorithm>

CTerrainVisibility::CTerrainVisibility(CTerrainObjectManager* objectManager, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth) 
	: _objectManager(objectManager)
{
	_fWorldScale = in_fWorldScale;
	_uiMaxDepth = in_uiMaxDepth;

	_aLodDistances[0] = in_fMaximumDistance;

	for (size_t i = 1; i < MAX_LODS; i++)
	{
		_aLodDistances[i] = _aLodDistances[i - 1] * in_fLodDistCoeff;
	}

	for (size_t i = 1; i < MAX_LODS; i++)
	{
		_aLodDistances[i] += 3000.0;
	}

	_vecRootObjects = objectManager->GetRootObjects();
}

void CTerrainVisibility::SetNewObjectHandler(const std::function<void(TerrainObjectID)>& newObjectHandler)
{
	_newObjectHandler = newObjectHandler;
}

void CTerrainVisibility::SetDeleteObjectHandler(const std::function<void(TerrainObjectID)>& deleteObjectHandler)
{
	_deleteObjectHandler = deleteObjectHandler;
}

void CTerrainVisibility::UpdateObjectsVisibility(float in_fDeltaTime, const vm::Vector3df& in_vPos)
{
	_setVisibleObjects.clear();

	vm::Vector3df vPos = vm::Vector3df(in_vPos[0] / _fWorldScale, in_vPos[1] / _fWorldScale, in_vPos[2] / _fWorldScale);

	if (_vecRootObjects.empty())
	{
		return;
	}

	double longitude, lattitude, height, length;
	GetWGS84LongLatHeight(vPos, longitude, lattitude, height, length);

	unsigned int uiMaxDepth = GetLodDepth(height);

	UpdateVisibleBlocks(vPos, uiMaxDepth);

	UpdateObjectsLifetime(in_fDeltaTime);
}

void CTerrainVisibility::CalculateLodDistances(float in_fCameraMeanFOV, const std::vector<size_t>& in_vecHFDimensions, const std::vector<float>& in_vecLodDiameter, unsigned int in_uiMeanScreenResolution, float in_uiPixelsPerTexel)
{
	size_t i;

	for (i = 0; i < in_vecHFDimensions.size(); i++)
	{
		if (in_vecLodDiameter[i] == 0)
			break;

		double dist = (double)in_vecLodDiameter[i] * in_uiMeanScreenResolution / (in_vecHFDimensions[i] * in_fCameraMeanFOV * in_uiPixelsPerTexel);

		_aLodDistances[i] = dist;
	}

	for (size_t j = i; j < MAX_LODS; j++)
	{
		_aLodDistances[j] = _aLodDistances[j - 1] * 0.5;
	}

	/*
	for (size_t i = 1; i < MAX_LODS; i++)
	{
		_aLodDistances[i] += 3000.0;
	}*/
}

void CTerrainVisibility::SetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods)
{
	for (size_t i = 0; i < in_nNLods; i++)
	{
		_aLodDistances[i] = in_aLodDistances[i] * 1000.0;
	}
}

void CTerrainVisibility::GetLodDistancesKM(double* in_aLodDistances, size_t in_nNLods)
{
	for (size_t i = 0; i < in_nNLods; i++)
	{
		in_aLodDistances[i] = _aLodDistances[i] * 0.001;
	}
}

double CTerrainVisibility::GetDistance(TerrainObjectID ID, const vm::Vector3df & in_vPos, double & out_Diameter)
{
	double dfLong, dfLat, dfHeight, dfLen;
	GetWGS84LongLatHeight(in_vPos, dfLong, dfLat, dfHeight, dfLen);

	STerrainBlockParams params;
	_objectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);


	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLong);
	double dfMidAngle = 0.5*(fabs(dfMaxLat - dfMinLat) + fabs(dfMaxLong - dfMinLong));
	out_Diameter = dfMidAngle * vm::length(vRefPoint);

	if (dfLat >= dfMinLat && dfLat <= dfMaxLat && dfLong >= dfMinLong && dfLong <= dfMaxLong)
	{
		return dfHeight;
	}

	if (dfLat >= dfMinLat && dfLat <= dfMaxLat)
	{
		double dfLongBorder = 0;

		if (AngularDistance(dfLong, dfMaxLong) > 0)
		{
			dfLongBorder = dfMaxLong;
		}
		else
		{
			dfLongBorder = dfMinLong;
		}

		vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfLongBorder, dfLat);
		return vm::length(vRefPoint - in_vPos);
	}

	if (dfLong >= dfMinLong && dfLong <= dfMaxLong)
	{
		double dfLatBorder = 0;

		if (AngularDistance(dfLat, dfMaxLat) > 0)
		{
			dfLatBorder = dfMaxLat;
		}
		else
		{
			dfLatBorder = dfMinLat;
		}

		vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfLong, dfLatBorder);
		return vm::length(vRefPoint - in_vPos);
	}

	std::vector<double> vecDists;

	vecDists.reserve(9);

	// corners
	vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMinLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMaxLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMinLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMaxLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	// mid points

	vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMidLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMidLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMinLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));

	vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMaxLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));
	// center

	vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLat);
	vecDists.push_back(vm::length(in_vPos - vRefPoint));


	std::sort(vecDists.begin(), vecDists.end(), std::less<double>());

	//out_Diameter = bbox.radius() * 2;

	return vecDists.front();
}

unsigned int CTerrainVisibility::GetLodDepth(double dist) const
{
	unsigned int uiDepth = 0;

	for (size_t i = 0; i < _uiMaxDepth; i++)
	{
		if (dist > _aLodDistances[i])
			break;

		uiDepth++;
	}

	if (uiDepth > 0)
		uiDepth--;

	return uiDepth ;
}

double CTerrainVisibility::AngularDistance(double a1, double a2)
{
	double cos1 = cos(a1);
	double cos2 = cos(a2);

	double sin1 = sin(a1);
	double sin2 = sin(a2);

	vm::Vector3df v1(cos1, sin1, 0);
	vm::Vector3df v2(cos2, sin2, 0);

	double angle = acos(vm::dot_prod(v1, v2));

	vm::Vector3df v = vm::cross(v1, v2);

	if (v[2] > 0)
		angle = -angle;

	return angle;
}

void CTerrainVisibility::UpdateVisibleBlocks(const vm::Vector3df & in_vPos, unsigned int uiMaxDepth)
{
	if (uiMaxDepth == 0)
	{
		for (TerrainObjectID rootID : _vecRootObjects)
			AddVisibleBlock(rootID);
	}
	else
	{
		for (TerrainObjectID rootID : _vecRootObjects)
		{
			UpdateVisibilityRecursive(rootID, in_vPos);
		}
	}
}

void CTerrainVisibility::AddVisibleBlock(TerrainObjectID ID)
{
	if (!_objectManager->IsObjectValid(ID))
		return;

	_setVisibleObjects.insert(ID);
}

CTerrainVisibility::EUpdateVisibilityResult CTerrainVisibility::UpdateVisibilityRecursive(TerrainObjectID ID, const vm::Vector3df & in_vPos)
{
	if (!_objectManager->IsObjectValid(ID))
		return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;

	bool bRes = false;

	double diameter = 0;
	double distToBlock = GetDistance(ID, in_vPos, diameter);
	unsigned int requiredLodDepth = GetLodDepth(distToBlock);

	unsigned char objectDepth = _objectManager->GetObjectDepth(ID);

	if (objectDepth == requiredLodDepth)
	{
		AddVisibleBlock(ID);

		return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
	}

	if (objectDepth < requiredLodDepth)
	{
		std::vector<TerrainObjectID> vecChildren;
		vecChildren.clear();

		_objectManager->GetTerrainObjectChildren(ID, vecChildren);

		if (vecChildren.size() == 0)
		{
			AddVisibleBlock(ID);

			return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}

		bool bSomeChildVisible = false;

		vector<TerrainObjectID> vecInvisibleChilds;
		vecInvisibleChilds.clear();

		vecInvisibleChilds.reserve(vecChildren.size());

		for (size_t i = 0; i < vecChildren.size(); i++)
		{
			TerrainObjectID childBlock = vecChildren[i];

			if (UpdateVisibilityRecursive(childBlock, in_vPos) == CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE)
			{
				bSomeChildVisible = true;
			}
			else
			{
				vecInvisibleChilds.push_back(childBlock);
			}
		}

		if (bSomeChildVisible)
		{
			for (TerrainObjectID invisibleChild : vecInvisibleChilds)
			{
				AddVisibleBlock(invisibleChild);
			}

			return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}
	}

	return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;
}

void CTerrainVisibility::RequestForAlive(TerrainObjectID ID)
{
	auto it = _mapAliveObjects.find(ID);

	// object not alive yet
	if (it == _mapAliveObjects.end())
	{
		_mapAliveObjects[ID] = SAliveObject(ID);

		// call handler
		if (_newObjectHandler)
			_newObjectHandler(ID);
	}
	else
	{
		// zero invisible time
		it->second.timeBeingInvisible = 0;
	}
}

void CTerrainVisibility::UpdateObjectsLifetime(float in_fDeltaTime)
{
	// 1. advance all objects invisible time
	for (std::map<TerrainObjectID, SAliveObject>::iterator it = _mapAliveObjects.begin(); it != _mapAliveObjects.end(); it++)
	{
		SAliveObject& aliveObject = it->second;
		aliveObject.timeBeingInvisible += in_fDeltaTime;
	}

	// 2. zero it for those, who visible, and create objects that are visible for the first time
	for (TerrainObjectID ID : _setVisibleObjects)
	{

		auto it = _mapAliveObjects.find(ID);

		// object not alive yet
		if (it == _mapAliveObjects.end())
		{
			_mapAliveObjects[ID] = SAliveObject(ID);

			// call handler
			if (_newObjectHandler)
				_newObjectHandler(ID);
		}
		else
		{
			// zero invisible time
			it->second.timeBeingInvisible = 0;
		}
	}


	// 3. check for dead objects
	for (std::map<TerrainObjectID, SAliveObject>::iterator it = _mapAliveObjects.begin(); it != _mapAliveObjects.end(); )
	{
		if (it->second.timeBeingInvisible > _fMaxInvisibleTime)
		{
			if (_deleteObjectHandler)
				_deleteObjectHandler(it->first);

			it = _mapAliveObjects.erase(it);
		}
		else
			it++;
	}
}
