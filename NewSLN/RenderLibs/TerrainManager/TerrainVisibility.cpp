#include "TerrainVisibility.h"

#include "Log.h"
#include "wgs84.h"

#include <algorithm>

CTerrainVisibility::CTerrainVisibility(CTerrainObjectManager* objectManager, CTerrainVisibilityOwner* owner, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth)
	: _objectManager(objectManager), _owner(owner)
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

	const double Rmin = 6356752.3142;
	if (vm::length(vPos) < Rmin*0.5)
	{
		LogMessage("CTerrainVisibility: position is below minimal radius of Earth (distance from Earth center: %d)", vm::length(vPos));
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
	vm::Vector3df vProjection, vNormal;
	_owner->GetTerrainObjectProjection(ID, in_vPos * _fWorldScale, vProjection, vNormal);
	vProjection *= 1.0 / _fWorldScale;

	double distance = vm::length(in_vPos - vProjection);

	out_Diameter = _owner->GetTerrainObjectDiameter(ID);
	
	// Эмперическое утверждение, но если мы находимся к объекту ближе чем на 3 диаметра, то рассчитываем 
	// кратчайшее расстояние более точно
	if (distance < 3 * out_Diameter)
	{
		_owner->GetTerrainObjectClosestPoint(ID, in_vPos * _fWorldScale, vProjection, vNormal);
		vProjection *= 1.0 / _fWorldScale;

		distance = vm::length(in_vPos - vProjection);
	}

	return distance;
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
	const std::function<bool(TerrainObjectID)>* in_pAdditionalCheckFunc, bool* out_bSubtreeDataReady)
{
	if (!_objectManager->IsObjectValid(ID))
	{
		if (out_bSubtreeDataReady)
			*out_bSubtreeDataReady = true;

		return CTerrainVisibility::EUpdateVisibilityResult::INVISIBLE;
	}

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

		if (out_bSubtreeDataReady)
		{
			*out_bSubtreeDataReady = _owner->IsDataReady(ID);
		}

		return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
	}

	if (objectDepth < requiredLodDepth)
	{
		std::vector<TerrainObjectID> vecChildren;

		_objectManager->GetTerrainObjectChildren(ID, vecChildren);

		if (vecChildren.size() == 0)
		{
			in_AddVisObjFunc(ID);

			if (out_bSubtreeDataReady)
			{
				*out_bSubtreeDataReady = _owner->IsDataReady(ID);
			}

			return CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}

		bool bSomeChildVisible = false;
		bool bChildrenSubtreeReady = true;

		vector<TerrainObjectID> vecInvisibleChilds;
		vecInvisibleChilds.reserve(vecChildren.size());

		for (size_t i = 0; i < vecChildren.size(); i++)
		{
			TerrainObjectID childBlock = vecChildren[i];

			bool bChildSubtreeReady = false;
			if (UpdateVisibilityRecursive(childBlock, in_vPos, in_AddVisObjFunc, in_pAdditionalCheckFunc, &bChildSubtreeReady) == CTerrainVisibility::EUpdateVisibilityResult::READY_AND_VISIBLE)
			{
				bSomeChildVisible = true;
			}
			else
			{
				vecInvisibleChilds.push_back(childBlock);
			}

			if (!bChildSubtreeReady)
				bChildrenSubtreeReady = false;
		}

		if (!bSomeChildVisible)
			bChildrenSubtreeReady = false;

		if (_owner->IsDrawingParentIfNotReadyChilds() && !bChildrenSubtreeReady)
		{
			in_AddVisObjFunc(ID);
		}

		if (out_bSubtreeDataReady)
		{
			if (bSomeChildVisible)
				*out_bSubtreeDataReady = bChildrenSubtreeReady;
			else
				*out_bSubtreeDataReady = _owner->IsDataReady(ID);
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

	if (out_bSubtreeDataReady)
	{
		*out_bSubtreeDataReady = _owner->IsDataReady(ID);
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
