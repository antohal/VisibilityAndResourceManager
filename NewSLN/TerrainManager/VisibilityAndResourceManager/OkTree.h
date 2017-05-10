#pragma once

#include "IGrid.h"

#include "Geometry/Plane.h"
#include "Containers/Store.h"
#include "GridIterator.h"

struct SIntersectStackData;
struct SDebugIntersectInfo;
struct STestingOkTreeObject;

enum EIntersectPlane { ipXY = 0, ipXZ, ipYZ };

const BYTE g_btAdressCount = 64 / 3; //21
const BYTE g_btDepthCount = g_btAdressCount + 1; //"0" Depth + other


struct SObjectInfo
{
	UINT_PTR	m_dwObject;
	vector<DWORD> m_vecIteratorTrace;

	SObjectInfo()
	{
		m_dwObject = 0;
	}
};

struct SObjectListElem
{
	DWORD				m_dwObjectIndex;
	SObjectListElem*	m_pNext;
	DWORD				m_dwIndexInStore;

	SObjectListElem()
	{
		m_dwObjectIndex = DWORD(-1);
		m_dwIndexInStore = DWORD(-1);
		m_pNext = NULL;
	}
};

struct COkNode
{
	union
	{
		SObjectListElem*	m_pOwnObject;
		DWORD				m_dwInfoIndex;
	};

	SObjectListElem*	m_pCommonLevelObjects;

	COkNode*			m_ppChildren[8];
	DWORD				m_dwIndexInStore;
	
	COkNode()
	{
		m_dwIndexInStore = DWORD(-1);
		m_pOwnObject = NULL;
		m_pCommonLevelObjects = NULL;
		ZeroMemory( m_ppChildren, sizeof(COkNode*) * 8 );
	}

	DWORD	GetInfoIndex() const { return m_dwInfoIndex - 1; }

	bool IsChildrenPresent() const
	{
		return m_ppChildren[0] != NULL || m_ppChildren[1] != NULL || m_ppChildren[2] != NULL || m_ppChildren[3] != NULL || m_ppChildren[4] != NULL ||
				m_ppChildren[5] != NULL || m_ppChildren[6] != NULL || m_ppChildren[7] != NULL;
	}
};

typedef vector<COkNode*> COkNodeVector;

struct STreeData
{
	COkNode*			pNode;
	__m64				mm64MinCur;
	__m64				mm64MaxCur;
	BYTE				btMinCell;
	BYTE				btMaxCell;
	BYTE				btDepth;
	Vector3D<DWORD>		vMin;
	BYTE				btCrossPlaneFlag;
};



struct SControlData
{
	CBoundBox<float>		m_BB; //обычный размер объекта
	DWORD					m_dwNodeSize; //размер минимальной €чейки (до какой будем спускатьс€)
	DWORD					m_dwObjectSize; //размер объекта в единицах сетки
	__m64					m_qwCoordMin;
	__m64					m_qwCoordMax;

	SControlData()
	{
		m_BB.Set( Vector3D<float>(0), Vector3D<float>(0) );
		m_dwObjectSize = 0;
		m_dwNodeSize = 0;
		m_qwCoordMin.m64_u64 = 0;//_mm_setzero_si64();
		m_qwCoordMax.m64_u64 = 0;//_mm_setzero_si64();
		//_mm_empty();
	}

	SControlData(const SControlData& in_Src)
	{
		m_BB = in_Src.m_BB;
		m_dwObjectSize = in_Src.m_dwObjectSize;
		m_dwNodeSize = in_Src.m_dwNodeSize;
		m_qwCoordMax = in_Src.m_qwCoordMax;
		m_qwCoordMin = in_Src.m_qwCoordMin;
		//_mm_empty();
	}

	SControlData(const CBoundBox<float>& in_BB, DWORD in_dwObjectSize, DWORD in_dwNodeSize, const __m64& in_qwCoordMin, const __m64& in_qwCoordMax)
	{
		m_BB = in_BB;
		m_dwObjectSize = in_dwObjectSize;
		m_dwNodeSize = in_dwNodeSize;
		m_qwCoordMax = in_qwCoordMax;
		m_qwCoordMin = in_qwCoordMin;
		//_mm_empty();
	}
};

class COkTree
{
public:
	COkTree(const Vector3D<float>& in_vMinCorner, const Vector3D<float>& in_vMaxCorner, BYTE in_btCommonDepth, BYTE in_btMaxDepth);
	~COkTree();

	virtual void					DrawDebug();

	unsigned						GetCommonDepth() const { return m_btCommonDepth; }
	void								EnumerateObjectsWithPoint(const Vector3D<float>& in_vPoint) { COkTree::for_each(in_vPoint,DummyPointCollector,0); }

	bool							RegisterObject(UINT_PTR in_dwObject, const CBoundBox<float>& in_BoundBox );
	bool							CollectNodeByAABB(const CBoundBox<float>& in_BoundBox, COkNodeVector& out_OkNodeVector );
	bool							UnregisterObject(UINT_PTR in_dwObject, const CBoundBox<float>& in_BoundBox );
	bool							MoveObject(UINT_PTR in_dwObject, const CBoundBox<float>& in_bbSource, const CBoundBox<float>& in_bbDestination );

	virtual IGridIterator&			GetIterator(const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum);

protected:	
	unsigned						m_iObjectInfoTraceSize;	
	bool							m_bCommonLevelApply;

	BYTE							GetDepthForMinNodeSize(DWORD in_dwMinNodeSize);

	virtual void					Update(float in_fGameTime) = 0; //for Debug
    
	//void							EnumerateObjectsWithPoint(const Vector3D<float>& in_vPoint);
	void							for_each( const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis );

	virtual void					OnCreateNode(COkNode* in_pNode, const Vector3D<DWORD>& in_vMin, BYTE in_btDepth) {}
	virtual void					OnDeleteNode(COkNode* in_pNode) {}
	void							GetAABBByNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, CBoundBox<float>& out_AABB);

	virtual DWORD					GetMinNodeSize(DWORD in_dwMaxObjectSize);
	DWORD							GetSizeInDWORD(float in_fSize);
	virtual DWORD					GetSaveIDByObjectID(UINT_PTR in_dwID) = 0;
	virtual UINT_PTR				GetObjectIDBySaveID(DWORD in_dwID) = 0;

	UINT_PTR						IsIntersectSegment(const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) const;

	template <class Type>
	bool							CollectCell( const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum, DWORD in_dwMinNodeSize, Type& out_aNodes );

	
	const Vector3D<double>&			GetMinCorner() const { return m_vMinCorner; }
	const Vector3D<double>&			GetMaxCorner() const { return m_vMaxCorner; }
	Vector3D<double>				GetSize() const { return m_vMaxCorner - m_vMinCorner; }

	virtual bool					CheckObjectByPoint(UINT_PTR in_dwObject, const Vector3D<float>& in_vPoint) = 0; //for EnumerateObjectsWithPoint //return true, if stop enumerate.
	
	void Clear();

	struct SOkNodeStackData
	{
		__m64						m_mm64MinCoord;
		__m64						m_mm64MaxCoord;
		COkNode*					m_pNode;
		Vector3D<DWORD>				m_vMin;
		BYTE						m_btDepth;
		BYTE						m_btCrossPlaneFlag;
	};
	
	struct SDelNodeData
	{
		COkNode*					m_pNode;
		COkNode*					m_pParent;
		BYTE						m_btIndexInParent;
	};

	struct SIntersectStackData
	{
		BYTE						btCurrIndex;
		const COkNode*				pCurrParent;
		Vector3D<double>			vCurrT0;
		Vector3D<double>			vCurrHalfSize;
		Vector3D<double>			vCurrCorner;
		Vector3D<double>			vCurrRealHalfSize;

		BYTE						m_btDepth; //for Debug
	};

	virtual bool					IsObjectIntersectSegment(UINT_PTR in_pObject, const SIntersectInputForObject& in_IntersectInput, SIntersectOut& in_IntersectOut) const = 0; //dT can be < 1!!!

	template<class T> class fixed_vector
	{
		T* const _buffer;
		const size_t _buffer_size;
		size_t _size;
	public:
		fixed_vector(T* buffer, size_t buffer_size) :
		  _buffer(buffer), 
			  _buffer_size(buffer_size),
			  _size(0)
		  {

		  }

		  void pop_back()
		  {
			  --_size;
		  }

		  void clear()
		  {
			  _size = 0;
		  }

		  T* AddEntries()
		  {
			  assert_debug(_size < _buffer_size, L"");
			  if(_size == _buffer_size)
				  throw std::overflow_error("fixed_vector overflow");
			  return _buffer + _size++;
		  }

		  T& back()
		  {
			  return _buffer[_size - 1];
		  }

		  bool empty() const
		  {
			  return ! _size;
		  }

		  size_t size() const
		  {
			  return _size;
		  }
	};

	typedef fixed_vector<SOkNodeStackData> TTmpStack;
	typedef fixed_vector<SDelNodeData> TTmpDelQueue;
	typedef vector<STestingOkTreeObject*> TTmpIntersectObjects;

private:	
	//mutable vector<SDebugIntersectInfo> m_aDebugIntersectInfo;

	unsigned							m_btCommonDepth;

	COkNode*							m_pRootNode;

	Vector3D<double>					m_vMinCorner;
	Vector3D<double>					m_vMaxCorner;
	Vector3D<double>					m_vMetersPerDWORD;
	Vector3D<double>					m_vDWORDperMeter;

	BYTE								m_btMaxDepth;

	CStore<COkNode>						m_stNodes;
	CStore<SObjectListElem>				m_stObjectListElems;
	CStore<SObjectInfo>					m_stObjectInfo;	

#ifdef _DEBUG
	std::map<UINT_PTR, SControlData>	m_mapControl;
#endif //_DEBUG
	
	//static CVector<SOkNodeStackData>	s_aStack;
	//static CVector<SDelNodeData>			s_aDelQueue;

	auto_ptr<IGridIterator>				m_pGridIterator;

	COkNode*							GetNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth);

	SObjectInfo*						GetObjectInfoByIndex(DWORD in_dwIndex);
	const SObjectInfo*					GetObjectInfoByIndex(DWORD in_dwIndex) const;

	IGridIterator&						AcquireIterator();

	DWORD								GetIndexForNewObject(UINT_PTR in_dwObject);
	DWORD								GetMinByChildNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, BYTE in_btChildIndex, Vector3D<DWORD>& out_vNewMin); //return Width

	void								GetAABBByChildNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, BYTE in_btChildIndex, Vector3D<DWORD>& out_vNewMin, CBoundBox<float>& out_AABB);
	void								AddObjectToNodeList(COkNode* in_pNode, DWORD in_dwObjectIndex, bool in_bCommonLevel);
	void								DrawNode(DWORD in_dwX, DWORD in_dwY, DWORD in_dwZ, DWORD in_dwDepth, COkNode* in_pCurNode);
	void								RemoveNode(COkNode* in_pNode);
	void								RemoveObjectByIndex(DWORD in_dwIndex);
	void								SetInTree(DWORD in_dwObjectIndex, STreeData& in_TreeData, BYTE in_btMinDepth);
	void								CreateAndCollectNode(STreeData& in_TreeData, BYTE in_btMinDepth, COkNodeVector& out_OkNodeVector );
	virtual void						RemoveFromTree(UINT_PTR in_dwObject, STreeData& in_TreeData, BYTE in_btMinDepth, DWORD& io_dwDeletedObjectIndex);

	template <class Type> void			CollectCell(STreeData& in_TreeData, BYTE in_btMinDepth, const CBoundBox<DWORD>& in_dwBoundBox, const CFrustum<float>* in_pFrustum, Type& out_aNodes);

	UINT_PTR							CheckIntersectInNode(const COkNode* const in_pNode, const SIntersectInputForObject& in_IntersectInput,
															Vector3D<double>& io_vPartSegmentBeforeIntersectObject,
															SIntersectOut& out_ReturnInfo) const;
	

	UINT_PTR 							IsIntersectSegmentInNode(const SIntersectInputForObject& in_IntersectInput, const COkNode* const in_pNode, SIntersectOut& out_ReturnInfo) const;

	bool								DeleteObjectFromNodeList(COkNode* in_pNode, UINT_PTR in_dwObject, DWORD& io_dwDeletedObjectIndex, bool in_bCommonLevel);

	bool								GetCoordInCell(const CBoundBox<float>& in_BoundBox, __m64& out_mm64Min, __m64& out_mm64Max, DWORD* out_pdwMaxObjectSize, CBoundBox<DWORD>* out_pdwRect) const;
	bool								GetCoordInCell(const Vector3D<float>& in_vPoint, __m64& out_mm64Coord) const;
	bool								CheckCoord(const Vector3D<float>& in_vPoint) const;
	bool								EnumerateNodeObjectsWithPoint(COkNode* in_pNode, const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis ); //return true, if stop enumerate.	
	bool								IsObjectInNode(COkNode* in_pNode, UINT_PTR in_dwObject, bool in_bCommonLevel) const;

	static bool					DummyPointCollector( IGridObject* in_pObj, UINT_PTR in_pThis )
	{
		return true;
	}

	friend struct SIteratorInfo;
	friend class CSingleGridIterator;
};


bool									IsObjectInNode(COkNode* in_pNode, DWORD in_dwObject);
DWORD									GetObjectCountInNode(COkNode* in_pNode, bool in_bCommonLevel);


__forceinline DWORD GetObjectListElemLength(SObjectListElem* in_pFirst)
{
	DWORD dwL = 0;
	SObjectListElem* pN = in_pFirst;
	while(pN)
	{
		dwL++;
		pN = pN->m_pNext;
	}
	return dwL;
}

inline SObjectInfo* COkTree::GetObjectInfoByIndex(DWORD in_dwIndex)
{
	SObjectInfo* pOI = m_stObjectInfo.GetPointer(in_dwIndex);
	assert_debug(pOI, L"Illegal poi");
	return pOI;
}

inline const SObjectInfo* COkTree::GetObjectInfoByIndex(DWORD in_dwIndex) const
{
	const SObjectInfo* pOI = m_stObjectInfo.GetPointer(in_dwIndex);
	assert_debug(pOI, L"Illegal poi");
	return pOI;
}

