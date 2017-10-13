#pragma once

#include "IGrid.h"
#include "OkTree.h"
	
class IGridIterator;
interface IGame;

class CEngineGrid: public IEngineGrid, public COkTree
{
public:
	CEngineGrid(float in_fWorldRadius, BYTE in_btMaxDepth);
	~CEngineGrid();

	//virtual bool					RegisterObject(IGridObject* in_pObject);
	//virtual bool					UnRegisterObject(IGridObject* in_pObject);

	virtual bool					CheckObjectByPoint(UINT_PTR in_dwObject, const Vector3D<float>& in_vPoint); //for EnumerateObjectsWithPoint //return true, if stop enumerate.
	virtual bool					IsObjectIntersectSegment(UINT_PTR in_pObject, const SIntersectInputForObject& in_IntersectInput, SIntersectOut& in_IntersectOut) const; //dT can be < 1!!!

	//virtual void					MoveObject(IGridObject* in_pObject, const CBoundBox<float>& in_bbSource, const CBoundBox<float>& in_bbDestination);

	virtual void					for_each( const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum, GridObjectProcess in_pFunc, UINT_PTR in_pThis );
	virtual void					for_each( const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis );

	virtual void					Update(float in_fGameTime) {} //for Debug

	virtual DWORD					GetSaveIDByObjectID(UINT_PTR in_dwID){ return 0; }
	virtual UINT_PTR				GetObjectIDBySaveID(DWORD in_dwID) { return 0; }

	virtual IGridObject*			IsIntersectSegment(const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) const;
};

class CEngineGridManager : public IEngineGridManager		
{
public:
	CEngineGridManager(float in_fWorldRadius);
	~CEngineGridManager();

	IEngineGrid*					GetGrid(size_t in_dwGridID);
	size_t							GetGridNum() const { return m_vecGrids.size(); } 
	IGridObject*					IsIntersectSegment(const std::vector<UINT_PTR>& in_vecGrids, const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo);

	void							Destroy();

private:

	vector<CEngineGrid*>			m_vecGrids;
	float							m_fWorldRadius;
};