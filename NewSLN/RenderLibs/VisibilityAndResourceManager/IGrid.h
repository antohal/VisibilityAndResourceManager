#pragma once

struct IGridObject;
//true - stop for_each
typedef bool (*GridObjectProcess)( IGridObject* in_pObject, UINT_PTR in_nUserParam);

enum EIntersectFlag
{
	INTERSECT_GEOMETRY = 0x00000001,
	INTERSECT_DONT_RETURN_POINT = 0x00000002
};

struct SIntersectInput
{
	Vector3D<float>				m_vStart;
	Vector3D<float>				m_vEnd;
	GridObjectProcess			m_pUserSkipFunc;
	UINT_PTR					m_pThis;

	UINT32						m_dwFlags;

	const void*					m_pUserParam;

	SIntersectInput():
		m_vStart(0),
		m_vEnd(0),
		m_pUserParam(NULL),
		m_pThis(NULL),
		m_pUserSkipFunc(NULL),
		m_dwFlags(0)
	{}



	SIntersectInput( const Vector3D<float>& in_vStart, const Vector3D<float>& in_vEnd, UINT_PTR in_nUserParam = 0, GridObjectProcess	in_pUserSkipFunc = 0, UINT32 in_dwFlags = 0):
		m_vStart(in_vStart),
		m_vEnd(in_vEnd),
		m_pUserParam(NULL),
		m_pThis(in_nUserParam),
		m_pUserSkipFunc(in_pUserSkipFunc),
		m_dwFlags(in_dwFlags)
	{}
};

struct SIntersectResultData
{
	union
	{
		struct
		{
			WORD m_uiTriangle;
		};
		UINT_PTR m_uiObject;
	};
	SIntersectResultData()
	{
		ZeroMemory( &m_uiTriangle, sizeof(SIntersectResultData));
	}

};

struct SIntersectOut
{
	double						m_dT; //in_out!!! out_dT - must be returned
	Vector3D<float>				m_vNormal;
	SIntersectResultData		m_ResultData;

	SIntersectOut():
	m_dT(1), m_vNormal(0)
	{
		int i = 0;
	}
};

struct SIntersectInputForObject: public SIntersectInput
{
	double						m_dblLength;
	double						m_dblInvLength;
	Vector3D<double>			m_vInvD;
	Vector3D<bool>				m_vInfinite;

	SIntersectInputForObject():
		m_dblLength(0),
		m_dblInvLength(0),
		m_vInfinite(false)
	{}

	SIntersectInputForObject(const SIntersectInput& in):
		SIntersectInput(in),
		m_dblLength(0),
		m_dblInvLength(0),
		m_vInfinite(false)
	{}
};

interface IGridObjectOwner
{
	virtual void					GetGridID(vector<int>& out_vecGridIDs) = 0;
};

interface IGridObject
{
	virtual CBoundBox<float>		GetBoundBox() const = 0;
	virtual bool					IsInPoint(const Vector3D<float>& in_vPoint) const = 0; //for EnumerateObjectsWithPoint //return true, if stop enumerate.
	virtual bool					IsIntersectSegment(const SIntersectInputForObject& in_IntersectInput, SIntersectOut& in_IntersectOut) const = 0; //dT can be < 1!!!
	virtual bool					IsVisibleInFrustum(const CFrustum<float>& in_Frustum, BYTE in_btCrossPlaneFlag) const = 0;
};

interface IGridObjectOBB
{
	virtual bool					Init(const Vector3D<float>& in_vNewPosition, const Matrix3x3<float>& in_vNewOrientation, const Vector3D<float>& in_vHalfSizes) = 0;
	virtual void					SetTransform(const Vector3D<float>& in_vNewPosition, const Matrix3x3<float>& in_vNewOrientation) = 0;
	virtual void					SetHalfSizes(const Vector3D<float>& in_vHalfSizes) = 0;
};

interface IGridObjectAABB
{
	virtual bool					Init(const CBoundBox<float>& in_BoundBox) = 0;
	virtual void					SetBoundBox(const CBoundBox<float>& in_BoundBox) = 0;
};


interface IEngineGrid
{
	//virtual bool					RegisterObject(IGridObject* in_pObject) = 0;
	//virtual bool					UnRegisterObject(IGridObject* in_pObject) = 0;

	//virtual void					MoveObject(IGridObject* in_pObject, const CBoundBox<float>& in_bbSource, const CBoundBox<float>& in_bbDestination) = 0;

	virtual void					for_each(const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum, GridObjectProcess in_pFunc, UINT_PTR in_pThis ) = 0;
	virtual void					for_each(const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis ) = 0;

	virtual IGridObject*			IsIntersectSegment(const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) const = 0;
};


interface IEngineGridManager
{
	static IEngineGridManager*		CreateInstance (float in_fWorldRadius);
	virtual void					Destroy() = 0;

	virtual IEngineGrid*			GetGrid(size_t in_dwGridID) = 0;
	virtual size_t					GetGridNum() const = 0;

	virtual IGridObject*			IsIntersectSegment(const std::vector<UINT_PTR>& in_vecGrids, const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) = 0;
};