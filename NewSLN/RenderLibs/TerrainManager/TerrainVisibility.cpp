#include "TerrainVisibility.h"

#include "Log.h"
#include "wgs84.h"

#include <algorithm>

CTerrainVisibility::CTerrainVisibility(CTerrainObjectManager* objectManager, CTerrainGeometryCalculator* geometryCalculator, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth)
	: _objectManager(objectManager), _geometryCalculator(geometryCalculator)
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

void CTerrainVisibility::CalculateLodDistanceCoeff(double height)
{
	_dfDistancesCoeff = 1.f;

	if (_dfLastLODDistanceOnEarth <= 0)
		return;

	double heightAboveEarth = fabs(height);

	if (heightAboveEarth < _aLodDistances[_uiMaxDepth])
	{
		double kMax = 1.0;
		double kMin = _dfLastLODDistanceOnEarth / _aLodDistances[_uiMaxDepth];

		if (kMin > 1.0)
			kMin = 1.0;

		_dfDistancesCoeff = vm::lerp(heightAboveEarth / _aLodDistances[_uiMaxDepth], kMin, kMax);
	}
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

	CalculateLodDistanceCoeff(height);

	unsigned int uiMaxDepth = GetLodDepth(height);

	UpdateVisibleBlocks(vPos, uiMaxDepth);

	_uiLastMaxDepth = uiMaxDepth;
	_vLastPos = in_vPos;
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
	STerrainBlockParams params;

	_objectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);


	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLong);
	double dfMidAngle = 0.5*(fabs(dfMaxLat - dfMinLat) + fabs(dfMaxLong - dfMinLong));
	out_Diameter = dfMidAngle * vm::length(vRefPoint);

	vm::Vector3df vProjection, vNormal;
	//_geometryCalculator->GetTerrainObjectProjection(ID, in_vPos * _fWorldScale, vProjection, vNormal);
	_geometryCalculator->GetTerrainObjectClosestPoint(ID, in_vPos * _fWorldScale, vProjection, vNormal);

	vProjection *= 1.0 / _fWorldScale;

	return vm::length(in_vPos - vProjection);

	//double dfLong, dfLat, dfHeight, dfLen;
	//GetWGS84LongLatHeight(in_vPos, dfLong, dfLat, dfHeight, dfLen);

	//STerrainBlockParams params;
	//_objectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	//double dfMinLat = params.fMinLattitude;
	//double dfMaxLat = params.fMaxLattitude;
	//double dfMinLong = params.fMinLongitude;
	//double dfMaxLong = params.fMaxLongitude;

	//double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	//double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);


	//vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLong);
	//double dfMidAngle = 0.5*(fabs(dfMaxLat - dfMinLat) + fabs(dfMaxLong - dfMinLong));
	//out_Diameter = dfMidAngle * vm::length(vRefPoint);

	//if (dfLat >= dfMinLat && dfLat <= dfMaxLat && dfLong >= dfMinLong && dfLong <= dfMaxLong)
	//{
	//	return dfHeight;
	//}

	//if (dfLat >= dfMinLat && dfLat <= dfMaxLat)
	//{
	//	double dfLongBorder = 0;

	//	if (_objectManager->AngularDistance(dfLong, dfMaxLong) > 0)
	//	{
	//		dfLongBorder = dfMaxLong;
	//	}
	//	else
	//	{
	//		dfLongBorder = dfMinLong;
	//	}

	//	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfLongBorder, dfLat);
	//	return vm::length(vRefPoint - in_vPos);
	//}

	//if (dfLong >= dfMinLong && dfLong <= dfMaxLong)
	//{
	//	double dfLatBorder = 0;

	//	if (_objectManager->AngularDistance(dfLat, dfMaxLat) > 0)
	//	{
	//		dfLatBorder = dfMaxLat;
	//	}
	//	else
	//	{
	//		dfLatBorder = dfMinLat;
	//	}

	//	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfLong, dfLatBorder);
	//	return vm::length(vRefPoint - in_vPos);
	//}

	//static std::vector<double> vecDists;
	//vecDists.resize(9);

	//// corners
	//vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMinLat);
	//vecDists[0] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMaxLat);
	//vecDists[1] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMinLat);
	//vecDists[2] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMaxLat);
	//vecDists[3] = (vm::length(in_vPos - vRefPoint));

	//// mid points

	//vRefPoint = GetWGS84SurfacePoint(dfMaxLong, dfMidLat);
	//vecDists[4] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMinLong, dfMidLat);
	//vecDists[5] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMinLat);
	//vecDists[6] = (vm::length(in_vPos - vRefPoint));

	//vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMaxLat);
	//vecDists[7] = (vm::length(in_vPos - vRefPoint));
	//// center

	//vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLat);
	//vecDists[8] = (vm::length(in_vPos - vRefPoint));


	//std::sort(vecDists.begin(), vecDists.end(), std::less<double>());

	//return vecDists.front();
}

unsigned int CTerrainVisibility::GetLodDepth(double dist) const
{
	unsigned int uiDepth = 0;

	for (size_t i = 0; i < _uiMaxDepth; i++)
	{
		if (dist > _aLodDistances[i] * _dfDistancesCoeff)
			break;

		uiDepth++;
	}

	if (uiDepth > 0)
		uiDepth--;

	return uiDepth ;
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
			UpdateVisibilityRecursive(rootID, in_vPos, [this](TerrainObjectID ID) {AddVisibleBlock(ID); }, nullptr);
		}
	}
}

void CTerrainVisibility::AddVisibleBlock(TerrainObjectID ID)
{
	if (!_objectManager->IsObjectValid(ID))
		return;

	_setVisibleObjects.insert(ID);
}

CTerrainVisibility::EUpdateVisibilityResult CTerrainVisibility::UpdateVisibilityRecursive(TerrainObjectID ID, const vm::Vector3df & in_vPos,
	const std::function<void(TerrainObjectID)>& in_AddVisObjFunc,
	const std::function<bool(TerrainObjectID)>* in_pAdditionalCheckFunc)
{
	if (!_objectManager->IsObjectValid(ID))
		return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;

	if (in_pAdditionalCheckFunc && !(*in_pAdditionalCheckFunc)(ID))
		return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;

	bool bRes = false;

	double diameter = 0;
	double distToBlock = GetDistance(ID, in_vPos, diameter);
	unsigned int requiredLodDepth = GetLodDepth(distToBlock);

	unsigned char objectDepth = _objectManager->GetObjectDepth(ID);

	if (objectDepth == requiredLodDepth)
	{
		in_AddVisObjFunc(ID);
		return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
	}

	if (objectDepth < requiredLodDepth)
	{
		std::vector<TerrainObjectID> vecChildren;

		_objectManager->GetTerrainObjectChildren(ID, vecChildren);

		if (vecChildren.size() == 0)
		{
			in_AddVisObjFunc(ID);

			return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}

		bool bSomeChildVisible = false;

		vector<TerrainObjectID> vecInvisibleChilds;
		vecInvisibleChilds.reserve(vecChildren.size());

		for (size_t i = 0; i < vecChildren.size(); i++)
		{
			TerrainObjectID childBlock = vecChildren[i];

			if (UpdateVisibilityRecursive(childBlock, in_vPos, in_AddVisObjFunc, in_pAdditionalCheckFunc) == CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE)
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
				in_AddVisObjFunc(invisibleChild);
			}

			return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}
	}

	return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;
}

bool CTerrainObjectVisibleSubtree::getReadyAndVisibleChildrenRecursive(TerrainObjectID ID, std::vector<TerrainObjectID>& out_vecReadyAndVisible, const std::set<TerrainObjectID>& setVisObjects, const std::set<TerrainObjectID>& setDataReadyObjects) const
{
	std::vector<TerrainObjectID> vecChildObjects;

	bool allChildrenReady = true;

	_pObjectManager->GetTerrainObjectChildren(ID, vecChildObjects);
	for (TerrainObjectID childID : vecChildObjects)
	{
		bool isVisible = setVisObjects.find(childID) != setVisObjects.end();
		bool isReady = setDataReadyObjects.find(childID) != setDataReadyObjects.end();

		if (isVisible && isReady)
			out_vecReadyAndVisible.push_back(childID);
		else
		if (!isVisible && _pObjectManager->GetObjectDepth(childID) < _uiLastMaxDepth)
			isReady = getReadyAndVisibleChildrenRecursive(childID, out_vecReadyAndVisible, setVisObjects, setDataReadyObjects);

		if (!isReady)
		{
			allChildrenReady = false;
			break;
		}
	}

	return allChildrenReady;
}

void CTerrainObjectVisibleSubtree::update(const std::set<TerrainObjectID>& setVisObjects, const std::set<TerrainObjectID>& setDataReadyObjects)
{
	std::vector<TerrainObjectID> vecObjsToDelete;
	std::vector<TerrainObjectID> vecObjsToInsert;

	// При построении алгоритма учитывалось одно простое правило, которое должно соблюдаться: то что если виден парент, то чилды не могут быть видны
	// и наоборот. Это касается как списка видимых объектов, так и списка предварительно видимых объектов

	for (TerrainObjectID ID : _setObjects)
	{
		//1. Если в блоке есть хоть один чилд, который и видим и готов, то разбиваем, если все чилды готовы:

		std::vector<TerrainObjectID> vecReadyAndVisibleChilds;

		bool allChildrenReady = getReadyAndVisibleChildrenRecursive(ID, vecReadyAndVisibleChilds, setVisObjects, setDataReadyObjects);
		bool hasVisibleAndReadyChildren = !vecReadyAndVisibleChilds.empty();

		if (hasVisibleAndReadyChildren && allChildrenReady)
		{
			vecObjsToInsert.insert(vecObjsToInsert.end(), vecReadyAndVisibleChilds.begin(), vecReadyAndVisibleChilds.end());
			vecObjsToDelete.push_back(ID);
		}
	}

	for (TerrainObjectID ID : vecObjsToDelete)
		_setObjects.erase(ID);

	for (TerrainObjectID ID : vecObjsToInsert)
		_setObjects.insert(ID);
}
