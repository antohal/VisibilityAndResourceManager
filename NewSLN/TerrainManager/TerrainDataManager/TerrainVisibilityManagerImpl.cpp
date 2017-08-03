#include "TerrainVisibilityManagerImpl.h"
#include "Log.h"
#include "wgs84.h"

#include <algorithm>


CTerrainVisibilityManager::CTerrainVisibilityManager()
{
	_implementation = new CTerrainVisibilityManagerImpl;
}

CTerrainVisibilityManager::~CTerrainVisibilityManager()
{
	delete _implementation;
}

void CTerrainVisibilityManager::Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth)
{
	_implementation->Init(in_pMeshTree, in_fWorldScale, in_fMaximumDistance, in_fLodDistCoeff, in_uiMaxDepth);
}


//@{ IVisibilityManagerPlugin
bool CTerrainVisibilityManager::IsObjectVisible(C3DBaseObject* in_pObject) const
{
	return _implementation->IsObjectVisible(in_pObject);
}

void CTerrainVisibilityManager::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_implementation->UpdateObjectsVisibility(in_vPos, in_vDir, in_vUp, in_pmProjection);
}
unsigned int CTerrainVisibilityManager::GetVisibleObjectsCount() const
{
	return _implementation->GetVisibleObjectsCount();
}
C3DBaseObject * CTerrainVisibilityManager::GetVisibleObject(unsigned int i)
{
	return _implementation->GetVisibleObject(i);
}
//@}



double AngularDistance(double a1, double a2)
{
	double cos1 = cos(a1);
	double cos2 = cos(a2);

	double sin1 = sin(a1);
	double sin2 = sin(a2);

	vm::Vector3df v1(cos1, sin1, 0);
	vm::Vector3df v2(cos2, sin2, 0);

	double angle = acos(vm::dot(v1, v2));

	vm::Vector3df v = vm::cross(v1, v2);

	if (v[2] > 0)
		angle = -angle;

	return angle;
}

// Функция вычисления расстояния между блоком и точкой
double CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::GetDistance(const CTerrainBlockDesc* in_pTerrainBlock, const vm::Vector3df& in_vPos, double& out_Diameter)
{
	double dfLong, dfLat, dfHeight, dfLen;
	GetWGS84LongLatHeight(in_vPos, dfLong, dfLat, dfHeight, dfLen);

	if (in_pTerrainBlock == _pRoot)
	{
		return dfHeight;
	}

	double dfMinLat = in_pTerrainBlock->GetParams()->fMinLattitude;;
	double dfMaxLat = in_pTerrainBlock->GetParams()->fMaxLattitude;
	double dfMinLong = in_pTerrainBlock->GetParams()->fMinLongitude;
	double dfMaxLong = in_pTerrainBlock->GetParams()->fMaxLongitude;

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

void CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::Init(C3DBaseTerrainObjectManager * in_pMeshTree, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth)
{
	_fWorldScale = in_fWorldScale;
	_uiMaxDepth = in_uiMaxDepth;

	_fLodDistCoeff = in_fLodDistCoeff;
	_fMaximumDistance = in_fWorldScale * in_fMaximumDistance;

	_pRoot = in_pMeshTree->GetRootTerrainData();

	for (size_t i = 0; i < in_pMeshTree->GetObjectsCount(); i++)
	{
		C3DBaseObject* pObj = in_pMeshTree->GetObjectByIndex(i);
		if (const CTerrainBlockDesc* pBlockData = in_pMeshTree->GetTerrainDataForObject(pObj))
		{
			_mapTerrainBlockInfo[pObj] = pBlockData;
			_mapObjects[pBlockData] = pObj;
		}
	}
}

bool CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::IsObjectVisible(C3DBaseObject * in_pObject) const
{
	return _setVisibleObjects.find(in_pObject) != _setVisibleObjects.end();
}

unsigned int CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::GetLodDepth(double dist) const
{
	unsigned int uiMaxDepth = 0;
	double dfCurrentLodDist = _fMaximumDistance;

	while (dist*_fWorldScale <= dfCurrentLodDist)
	{
		uiMaxDepth++;
		dfCurrentLodDist *= _fLodDistCoeff;
	}

	return std::min<unsigned int>(_uiMaxDepth, uiMaxDepth);
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_setVisibleObjects.clear();

	vm::Vector3df vPos = vm::Vector3df(in_vPos.x / _fWorldScale, in_vPos.y / _fWorldScale, in_vPos.z / _fWorldScale);

	if (!_pRoot)
		return;

	double longitude, lattitude, height, length;
	GetWGS84LongLatHeight(vPos, longitude, lattitude, height, length);

	unsigned int uiMaxDepth = GetLodDepth(height);

	UpdateVisibleBlocks(vPos, uiMaxDepth);
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::UpdateVisibleBlocks(const vm::Vector3df& vPos, unsigned int uiMaxDepth)
{
	if (uiMaxDepth == 0)
	{
		for (unsigned int i = 0; i < _pRoot->GetChildBlockDescCount(); i++)
		{
			const CTerrainBlockDesc* pBlock = _pRoot->GetChildBlockDesc(i);
			AddVisibleBlock(pBlock, vPos);
		}
	}
	else
	{
		UpdateVisibilityRecursive(_pRoot, vPos);
	}
}

CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::UpdateVisibilityRecursive(const CTerrainBlockDesc* pTerrainBlock, const vm::Vector3df& in_vPos)
{
	bool bRes = false;

	double diameter = 0;
	double distToBlock = GetDistance(pTerrainBlock, in_vPos, diameter);
	unsigned int requiredLodDepth = GetLodDepth(distToBlock);

	if (pTerrainBlock->GetParams()->uiDepth == requiredLodDepth)
	{
		AddVisibleBlock(pTerrainBlock, in_vPos);

		return CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult::READY_AND_VISIBLE;
	}

	if (pTerrainBlock->GetParams()->uiDepth < requiredLodDepth)
	{
		if (pTerrainBlock->GetChildBlockDescCount() == 0)
		{
			AddVisibleBlock(pTerrainBlock, in_vPos);

			return CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}

		bool bSomeChildVisible = false;

		static vector<const CTerrainBlockDesc*> vecInvisibleChilds;
		vecInvisibleChilds.clear();

		vecInvisibleChilds.reserve(pTerrainBlock->GetChildBlockDescCount());

		for (unsigned int i = 0; i < pTerrainBlock->GetChildBlockDescCount(); i++)
		{
			const CTerrainBlockDesc* pChildBlock = pTerrainBlock->GetChildBlockDesc(i);

			if (UpdateVisibilityRecursive(pChildBlock, in_vPos) == CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult::READY_AND_VISIBLE)
			{
				bSomeChildVisible = true;
			}
			else
			{
				vecInvisibleChilds.push_back(pChildBlock);
			}
		}

		if (bSomeChildVisible)
		{
			for (const CTerrainBlockDesc* pInvisibleChild : vecInvisibleChilds)
			{
				AddVisibleBlock(pInvisibleChild, in_vPos);
			}

			return CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult::READY_AND_VISIBLE;
		}
	}

	return CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::EUpdateVisibilityResult::INVISIBLE;
}

//bool CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::IsBlockBehindEarth(const CTerrainBlockDesc* in_pTerrainBlock, const vm::Vector3df& in_vPos) const
//{
//
//	// TODO: здесь можно поместить обработку параметра по определению того какие блоки мы рисуем - либо фронтальные, либо задние
//
//	double dfMinLat = in_pTerrainBlock->GetParams()->fMinLattitude;
//	double dfMaxLat = in_pTerrainBlock->GetParams()->fMaxLattitude;
//	double dfMinLong = in_pTerrainBlock->GetParams()->fMinLongitude;
//	double dfMaxLong = in_pTerrainBlock->GetParams()->fMaxLongitude;
//
//	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
//	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);
//
//	double dfMinHeight = (-10000.0);
//	double dfMaxHeight = (10000.0);
//
//
//	double dfDeltaLat = (dfMaxLat - dfMinLat) / 10;
//	double dfDeltaLong = (dfMaxLong - dfMinLong) / 10;
//
//	for (double dfLat = dfMinLat; dfLat <= dfMaxLat; dfLat += dfDeltaLat)
//	{
//		for (double dfLong = dfMinLong; dfLong <= dfMaxLong; dfLong += dfDeltaLong)
//		{
//			vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfLong, dfLat);
//
//			//if (!IsSegmentIntersectsEarthMinRadius(in_vPos, vRefPoint))
//			//	return false;
//
//			if (!IsSegmentIntersectsEarthMinRadius(in_vPos, vRefPoint + vm::normalize(vRefPoint) * dfMinHeight))
//				return false;
//
//			if (!IsSegmentIntersectsEarthMinRadius(in_vPos, vRefPoint + vm::normalize(vRefPoint) * dfMaxHeight))
//				return false;
//		}
//	}
//
//	return true;
//}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::AddVisibleBlock(const CTerrainBlockDesc* pBlock, const vm::Vector3df& in_vPos)
{
	//if (!IsBlockBehindEarth(pBlock, in_vPos))
	_setVisibleObjects.insert(_mapObjects[pBlock]);
}

unsigned int CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::GetVisibleObjectsCount() const
{
	return _setVisibleObjects.size();
}

C3DBaseObject* CTerrainVisibilityManager::CTerrainVisibilityManagerImpl::GetVisibleObject(unsigned int i)
{
	std::set<C3DBaseObject*>& setObjects = _setVisibleObjects;

	if (i >= setObjects.size())
		return nullptr;

	unsigned int index = 0;

	for (C3DBaseObject* obj : setObjects)
	{
		if (index == i)
			return obj;

		index++;
	}

	return nullptr;
}