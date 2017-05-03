#include "TerrainVisibilityManager.h"
#include "Log.h"
#include "wgs84.h"

#include <algorithm>

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

	if (v[2] < 0)
		angle = -angle;

	return angle;
}

// ‘ункци€ вычислени€ рассто€ни€ между блоком и точкой
double GetDistance(const CTerrainBlockData* in_pTerrainBlock, const vm::Vector3df& in_vPos, double& out_Diameter)
{
	double dfLong, dfLat, dfHeight, dfLen;
	GetWGS84LongLatHeight(in_vPos, dfLong, dfLat, dfHeight, dfLen);

	double dfMinLat = in_pTerrainBlock->GetMinimumLattitude();
	double dfMaxLat = in_pTerrainBlock->GetMaximumLattitude();
	double dfMinLong = in_pTerrainBlock->GetMinimumLongitude();
	double dfMaxLong = in_pTerrainBlock->GetMaximumLongitude();
	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);


	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLong);
	double dfMidAngle = 0.5*(dfMaxLat - dfMinLat + dfMaxLong - dfMinLong);
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

CTerrainVisibilityManager::CTerrainVisibilityManager()
{
	_implementation = new CTerrainVisibilityManagerImplementation();
}

CTerrainVisibilityManager::~CTerrainVisibilityManager()
{
	delete _implementation;
}

void CTerrainVisibilityManager::Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale)
{
	_implementation->Init(in_pMeshTree, in_fWorldScale);
}

bool CTerrainVisibilityManager::IsObjectVisible(C3DBaseObject* in_pObject) const
{
	return _implementation->IsObjectVisible(in_pObject);
}

void CTerrainVisibilityManager::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_implementation->UpdateObjectsVisibility(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::Init(C3DBaseTerrainObjectManager * in_pMeshTree, float in_fWorldScale)
{
	_fWorldScale = in_fWorldScale;

	_pRoot = in_pMeshTree->GetRootTerrainData();

	for (size_t i = 0; i < in_pMeshTree->GetObjectsCount(); i++)
	{
		C3DBaseObject* pObj = in_pMeshTree->GetObjectByIndex(i);
		if (const CTerrainBlockData* pBlockData = in_pMeshTree->GetTerrainDataForObject(pObj))
		{
			_mapTerrainBlockInfo[pObj] = pBlockData;
			_mapObjects[pBlockData] = pObj;
		}
	}
}

bool CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::IsObjectVisible(C3DBaseObject * in_pObject) const
{
	return _setVisibleObjects.find(in_pObject) != _setVisibleObjects.end();
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_setVisibleObjects.clear();

	vm::Vector3df vPos = vm::Vector3df(in_vPos.x / _fWorldScale, in_vPos.y / _fWorldScale, in_vPos.z / _fWorldScale);

	if (!_pRoot)
		return;

	/*for (unsigned int i = 0; i < _pRoot->GetChildBlockDataCount(); i++)
		UpdateVisibilityRecursive(_pRoot->GetChildBlockData(i), vPos);*/

	UpdateVisibilityRecursive(_pRoot, vPos);
}

bool CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::UpdateVisibilityRecursive(const CTerrainBlockData* pTerrainBlock, const vm::Vector3df& in_vPos)
{
	// true возвращаем только в том случае, если действительно объект стал видимым

	const double k_dfDistCoeff = 0.5;

	double diameter = 0;
	double distance = 0;
	
	if (pTerrainBlock->GetParentBlockData())
	{
		distance = GetDistance(pTerrainBlock, in_vPos, diameter);

		// ≈сли рассто€ние больше дистанции видимости, то лод не видим
		if (distance > k_dfDistCoeff * diameter)
			return false;
	}

	bool bSomeChildVisible = false;

	for (unsigned int i = 0; i < pTerrainBlock->GetChildBlockDataCount(); i++)
	{
		if (UpdateVisibilityRecursive(pTerrainBlock->GetChildBlockData(i), in_vPos))
			bSomeChildVisible = true;
	}

	if (!bSomeChildVisible)
	{
		AddVisibleBlock(pTerrainBlock);

		return true;
	}
	else
	{

		for (unsigned int i = 0; i < pTerrainBlock->GetChildBlockDataCount(); i++)
			AddVisibleBlock(pTerrainBlock->GetChildBlockData(i));

	}

	return false;

	//bool bSomeChildVisible = false;
	//static std::vector<const CTerrainBlockData*> vecInvisibleChilds;
	//vecInvisibleChilds.resize(0);
	//for (unsigned int i = 0; i < pTerrainBlock->GetChildBlockDataCount(); i++)
	//{
	//	if (UpdateVisibilityRecursive(pTerrainBlock->GetChildBlockData(i), in_vPos))
	//		bSomeChildVisible = true;
	//	else
	//		vecInvisibleChilds.push_back(pTerrainBlock->GetChildBlockData(i));
	//}

	//// если никака€ из дочерних ветвей не видима, значит видимы мы сами
	//if (!bSomeChildVisible)
	//{
	//	if (pTerrainBlock->GetParentBlockData())
	//		AddVisibleBlock(pTerrainBlock);
	//	else
	//	{
	//		// если корневой блок - добавл€ем чилдов
	//		for (unsigned int i = 0; i < pTerrainBlock->GetChildBlockDataCount(); i++)
	//			AddVisibleBlock(pTerrainBlock->GetChildBlockData(i));
	//	}
	//
	//	/*if (pTerrainBlock->GetParentBlockData())
	//	{
	//		for (unsigned int i = 0; i < pTerrainBlock->GetParentBlockData()->GetChildBlockDataCount(); i++)
	//		{

	//		}
	//	}*/

	//	return true;
	//}
	//else
	//{
	//	for (const CTerrainBlockData* pInvisChildBlock : vecInvisibleChilds)
	//	{
	//		AddVisibleBlock(pInvisChildBlock);
	//	}
	//}

	//return bSomeChildVisible;
}

void CTerrainVisibilityManager::CTerrainVisibilityManagerImplementation::AddVisibleBlock(const CTerrainBlockData* pBlock)
{
	_setVisibleObjects.insert(_mapObjects[pBlock]);
}
