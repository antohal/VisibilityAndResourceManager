#include "stdafx.h"
#include "GridManager.h"
#include "GridIterator.h"

IEngineGridManager*		IEngineGridManager::CreateInstance (float in_fWorldRadius)
{
	return new CEngineGridManager(in_fWorldRadius);
}

CEngineGridManager::CEngineGridManager(float in_fWorldRadius) : m_fWorldRadius(in_fWorldRadius)
{
}

CEngineGridManager::~CEngineGridManager()
{

}

IEngineGrid* CEngineGridManager::GetGrid(size_t in_idwGridID)
{
	assert_debug(in_idwGridID < 500, L"Warning!!! ID is very large!");

	if(in_idwGridID >= m_vecGrids.size())
		m_vecGrids.resize(in_idwGridID + 1);

	if( m_vecGrids[in_idwGridID] == NULL )
		m_vecGrids[in_idwGridID] = new CEngineGrid(m_fWorldRadius, g_btDepthCount);

	return static_cast<IEngineGrid*>(m_vecGrids[in_idwGridID]);
}

void CEngineGridManager::Destroy()
{
	for(auto it = m_vecGrids.begin(); it != m_vecGrids.end(); it++)
		if (CEngineGrid* pGrid = *it)
			delete pGrid;

	m_vecGrids.clear();
}

IGridObject* CEngineGridManager::IsIntersectSegment(const std::vector<UINT_PTR>& in_vecGrids, const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo)
{
	IGridObject* pResult = 0;
	for( size_t nGrid = 0, nGridNum = in_vecGrids.size(); nGrid < nGridNum; nGrid++ )
	{
		IEngineGrid* pGrid = GetGrid(in_vecGrids[nGrid]);
		IGridObject* pNewResult = pGrid->IsIntersectSegment(in_IntersectInput, io_ReturnInfo );
		if( pNewResult)
			pResult = pNewResult;
	}
	return pResult;
}


//////////////////////////////////////////////////////////////////////////
// CEngineGrid
//////////////////////////////////////////////////////////////////////////

CEngineGrid::CEngineGrid(float in_fWorldRadius, BYTE in_btMaxDepth):
	COkTree( Vector3D<float>(-in_fWorldRadius), Vector3D<float>(in_fWorldRadius), 10, in_btMaxDepth)
{
}

CEngineGrid::~CEngineGrid()
{
}
//
//bool CEngineGrid::RegisterObject(IGridObject* in_pObject)
//{
//	UINT_PTR pObj = reinterpret_cast<UINT_PTR>(in_pObject);
//	return COkTree::RegisterObject( pObj, in_pObject->GetBoundBox() );
//}
//
//bool CEngineGrid::UnRegisterObject(IGridObject* in_pObject)
//{
//	UINT_PTR pObj = reinterpret_cast<UINT_PTR>(in_pObject);
//	return COkTree::UnregisterObject( pObj, in_pObject->GetBoundBox() );
//}

void CEngineGrid::for_each(const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum, GridObjectProcess in_pFunc, UINT_PTR in_pThis )
{
	//CSimplePerfomanceCounter::GetPerfomanceCounter().StartCount( L"CEngineGrid::for_each" );

	IGridIterator& GI = GetIterator(in_BoundBox, in_pFrustum);

	for(; !GI.IsEnd(); GI.Next() )
	{
		IGridObject* pObj = reinterpret_cast<IGridObject*>( GI.Get() );
		if( in_BoundBox.IsIntersectingAnotherBox( pObj->GetBoundBox() ) )
		{
			bool bInFrustum = true;
			if(in_pFrustum)
			{
				BYTE btCrossFrustumPlaneFlag = GI.GetFrustumFlag();
				bInFrustum = !btCrossFrustumPlaneFlag || pObj->IsVisibleInFrustum(*in_pFrustum, btCrossFrustumPlaneFlag);
			}

			if( bInFrustum && in_pFunc( pObj, in_pThis ) )
				break;
		}
	}

	//CSimplePerfomanceCounter::GetPerfomanceCounter().StopCount( L"CEngineGrid::for_each" );
}

void CEngineGrid::for_each( const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis )
{
	COkTree::for_each( in_vPoint, in_pFunc, in_pThis );
}
//
//void CEngineGrid::MoveObject(IGridObject* in_pObject, const CBoundBox<float>& in_bbSource, const CBoundBox<float>& in_bbDestination)
//{
//	UINT_PTR pObj = reinterpret_cast<UINT_PTR>(in_pObject);
//	COkTree::MoveObject(pObj, in_bbSource, in_bbDestination);
//}

IGridObject* CEngineGrid::IsIntersectSegment(const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) const
{
	return reinterpret_cast<IGridObject*>( COkTree::IsIntersectSegment( in_IntersectInput, io_ReturnInfo ) );
}

bool CEngineGrid::CheckObjectByPoint(UINT_PTR in_dwObject, const Vector3D<float>& in_vPoint)
{
	IGridObject* pGridObject = reinterpret_cast<IGridObject*>( in_dwObject );

	return pGridObject->IsInPoint( in_vPoint );
}

bool CEngineGrid::IsObjectIntersectSegment(UINT_PTR in_pObject, const SIntersectInputForObject& in_IntersectInput, SIntersectOut& in_IntersectOut) const
{
	IGridObject* pGridObject = reinterpret_cast<IGridObject*>( in_pObject );
	
	bool bUserSkip = false;
	if( in_IntersectInput.m_pThis && in_IntersectInput.m_pUserSkipFunc )
		bUserSkip = in_IntersectInput.m_pUserSkipFunc(pGridObject,in_IntersectInput.m_pThis);

	return !bUserSkip && pGridObject->IsIntersectSegment( in_IntersectInput, in_IntersectOut );
}
