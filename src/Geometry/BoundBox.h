// BoundBox.h

#pragma once

template <typename Type> inline Type GetMaxValue() { return Type(DBL_MAX); }
template<> inline float GetMaxValue<float>() { return FLT_MAX; }
template<> inline double GetMaxValue<double>() { return DBL_MAX; }
template<> inline DWORD GetMaxValue<DWORD>() { return static_cast<DWORD>(~0); }
template<> inline size_t GetMaxValue<size_t>() { return static_cast<size_t>(~0); }

template <typename Type> inline Type GetMinValue() { return Type(-DBL_MAX); }
template<> inline float GetMinValue<float>() { return -FLT_MAX; }
template<> inline double GetMinValue<double>() { return -DBL_MAX; }
template<> inline DWORD GetMinValue<DWORD>() { return 0; }
template<> inline size_t GetMinValue<size_t>() { return 0; }

template <typename Type>
class CBoundBox
{
public:

	union
	{
	struct
	{
		Vector3D<Type> vMin;
		Vector3D<Type> vMax;
	};
	struct
	{
		Vector3D<Type> m_vMin;
		Vector3D<Type> m_vMax;
	};
	};
	

	CBoundBox();
	CBoundBox(const CBoundBox& in_BoundBox);
	CBoundBox(const Vector3D<Type>& in_vMin, const Vector3D<Type>& in_vMax);
	CBoundBox(const Vector3D<Type>& in_vCenter, Type in_fHalfSize);
	CBoundBox(const Vector3D<Type>& in_vCenter, Type in_fHalfX, Type in_fHalfY, Type in_fHalfZ);

	void Init() { vMax = Vector3D<Type>( GetMinValue<Type>() ); vMin = Vector3D<Type>( GetMaxValue<Type>() ); };
	void Set(const Vector3D<Type> &in_vMin, const Vector3D<Type> &in_vMax)
	{
		assert_debug(in_vMin.x <= in_vMax.x && in_vMin.y <= in_vMax.y && in_vMin.z <= in_vMax.z, L"");

		vMin = in_vMin; vMax = in_vMax;
	};
	
	void SetCenterHalfSize(const Vector3D<Type>& in_vCenter, const Vector3D<Type>& in_vHalfSize);

	bool IsValid() const {return (vMax.x >= vMin.x) && (vMax.y >= vMin.y) && (vMax.z >= vMin.z);}

	bool IsDifferent (const CBoundBox<Type>& in_bbOther) const
	{
		return ! IsEqualBoundBox(*this, in_bbOther);
	}

	bool IsHasVolume() const { return (vMax.x > vMin.x) && (vMax.y > vMin.y) && (vMax.z > vMin.z); }

	Vector3D<Type> GetCenter() const {assert_debug(IsValid(), L""); return (vMax + vMin) * Type(0.5);}
	Vector3D<Type> GetHalfSize() const {assert_debug(IsValid(), L""); return (vMax - vMin) * Type(0.5);}
	Vector3D<Type> GetSize() const {assert_debug(IsValid(), L""); return vMax - vMin;}
	Type GetRadius() const {assert_debug(IsValid(), L""); return Length(vMax - vMin) * Type(0.5);}

	void Update(const Vector3D<Type> &in_vPointToInclude)
	{
		vMin.UpdateMin(in_vPointToInclude);
		vMax.UpdateMax(in_vPointToInclude);
	}

	void UpdateWithRadius(const Vector3D<Type> &in_vCenter, Type in_fRadius)
	{
		assert_debug(in_fRadius >= 0, L"");
		
		vMin.UpdateMin(in_vCenter - Vector3D<Type>(in_fRadius));
		vMax.UpdateMax(in_vCenter + Vector3D<Type>(in_fRadius));
	}

	void UpdateByBoundBox(const CBoundBox<Type> &in_BoundBox)
	{
		assert_debug(in_BoundBox.IsValid(), L"");

		vMin.UpdateMin(in_BoundBox.vMin);
		vMax.UpdateMax(in_BoundBox.vMax);
	}

	void UpdateByBoundBoxWithRadius(const CBoundBox<Type> &in_BoundBox, Type in_fRadius)
	{
		assert_debug(in_BoundBox.IsValid(), L"");

		vMin.UpdateMin(in_BoundBox.vMin - Vector3D<Type>(in_fRadius));
		vMax.UpdateMax(in_BoundBox.vMax + Vector3D<Type>(in_fRadius));
	}

	void UpdateByRotatedBoundBox(const CBoundBox<Type> &in_BoundBox, const Vector3D<Type>& in_vPosition, const Matrix3x3<Type>& in_mOrientation)
	{
		//FIXME_FOX: оптимизировать? (выбирать самые далекие точки)
		assert_debug(in_BoundBox.IsValid(), L"");

		Vector3D<Type> vBasePoint = in_vPosition + mul(in_BoundBox.GetCenter(), in_mOrientation);
		Vector3D<Type> vHS = in_BoundBox.GetHalfSize();

		Update(vBasePoint + mul(Vector3D<Type>( vHS.x, vHS.y, vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>(-vHS.x, vHS.y, vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>( vHS.x,-vHS.y, vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>(-vHS.x,-vHS.y, vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>( vHS.x, vHS.y,-vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>(-vHS.x, vHS.y,-vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>( vHS.x,-vHS.y,-vHS.z), in_mOrientation));
		Update(vBasePoint + mul(Vector3D<Type>(-vHS.x,-vHS.y,-vHS.z), in_mOrientation));
	}

	Vector3D<Type> PointOnSurfaceClosestTo(const Vector3D<float>& in_vPoint) const
	{
		Vector3D<Type> MPoint = Vector3D<Type>(0);

		MPoint.x = (in_vPoint.x < vMin.x) ? vMin.x : (in_vPoint.x > vMax.x) ? vMax.x : in_vPoint.x;
		MPoint.y = (in_vPoint.y < vMin.y) ? vMin.y : (in_vPoint.y > vMax.y) ? vMax.y : in_vPoint.y;
		MPoint.z = (in_vPoint.z < vMin.z) ? vMin.z : (in_vPoint.z > vMax.z) ? vMax.z : in_vPoint.z;

		return MPoint;
	}

	void UpdateByOBB(const Matrix3x3<Type>& in_mBBOrientation, const Vector3D<Type>& in_vBBHalfSizes, const Vector3D<Type>& in_vBBCenter)
	{
		Vector3D<Type> vAABBHalfSizes(TAbs<Type>( in_mBBOrientation._11 * in_vBBHalfSizes.x ) +
			TAbs<Type>( in_mBBOrientation._21 * in_vBBHalfSizes.y ) +
			TAbs<Type>( in_mBBOrientation._31 * in_vBBHalfSizes.z ),
			TAbs<Type>( in_mBBOrientation._12 * in_vBBHalfSizes.x ) +
			TAbs<Type>( in_mBBOrientation._22 * in_vBBHalfSizes.y ) +
			TAbs<Type>( in_mBBOrientation._32 * in_vBBHalfSizes.z ),
			TAbs<Type>( in_mBBOrientation._13 * in_vBBHalfSizes.x ) +
			TAbs<Type>( in_mBBOrientation._23 * in_vBBHalfSizes.y ) +
			TAbs<Type>( in_mBBOrientation._33 * in_vBBHalfSizes.z ));
		
		vMin.UpdateMin(in_vBBCenter - vAABBHalfSizes);
		vMax.UpdateMax(in_vBBCenter + vAABBHalfSizes);
	}


	void BoundByBoundBox(const CBoundBox<Type> &in_BoundBox)
	{
		assert_debug(in_BoundBox.IsValid(), L"");

		vMin.UpdateMax(in_BoundBox.vMin);
		vMax.UpdateMin(in_BoundBox.vMax);

		assert_debug(IsValid(), L"");
	}

	void BoundByMinMax(const Vector3D<Type>& in_vMin, const Vector3D<Type>& in_vMax)
	{
		vMin.UpdateMax(in_vMin);
		vMax.UpdateMin(in_vMax);
	}

	void BoundByTwoPnts(const Vector3D<Type>& in_v1, const Vector3D<Type>& in_v2)
	{
		vMin = vMax = in_v1;
		vMin.UpdateMin(in_v2);
		vMax.UpdateMax(in_v2);

		assert_debug(IsValid(), L"");
	}
	void Inflate(Type in_fRadius)
	{
		assert_debug(IsValid(), L"");

		vMin -= Vector3D<Type>(in_fRadius);
		vMax += Vector3D<Type>(in_fRadius);
	}

	void GetSphere(Vector3D<Type> &out_vCenter, Type &out_fRadius) const
	{
		assert_debug(IsValid(), L"");
		out_vCenter = (vMax + vMin) * 0.5f;
		out_fRadius = Length(vMax - vMin) * 0.5f;
	}

	bool IsContainingBox(const CBoundBox<Type>& in_InnerBoundBox, Type in_fEpsilon = Type(1e-4f)) const
	{
		return in_InnerBoundBox.vMin.x >= vMin.x - in_fEpsilon && in_InnerBoundBox.vMax.x <= vMax.x + in_fEpsilon
			&& in_InnerBoundBox.vMin.y >= vMin.y - in_fEpsilon && in_InnerBoundBox.vMax.y <= vMax.y + in_fEpsilon
			&& in_InnerBoundBox.vMin.z >= vMin.z - in_fEpsilon && in_InnerBoundBox.vMax.z <= vMax.z + in_fEpsilon;
	}

	bool IsPointInBox(const Vector3D<Type>& in_vPoint) const
	{
		return in_vPoint.x >= vMin.x && in_vPoint.x <= vMax.x
			&& in_vPoint.y >= vMin.y && in_vPoint.y <= vMax.y
			&& in_vPoint.z >= vMin.z && in_vPoint.z <= vMax.z;
	}

	bool IsPointInBox2D(const Vector3D<Type>& in_vPoint) const
	{
		return in_vPoint.x >= vMin.x && in_vPoint.x <= vMax.x
			&& in_vPoint.y >= vMin.y && in_vPoint.y <= vMax.y;
	}

	bool IsPointInEllipsoid(const Vector3D<Type>& in_vPoint) const
	{
		const Vector3D<Type> vRadii = GetHalfSize();
		const Vector3D<Type> vSqrRadii(sqr(vRadii.x), sqr(vRadii.y), sqr(vRadii.z));
		const Vector3D<Type> vPoint = in_vPoint - GetCenter();

		return sqr(vPoint.x) * vSqrRadii.y * vSqrRadii.z
			 + sqr(vPoint.y) * vSqrRadii.x * vSqrRadii.z
			 + sqr(vPoint.z) * vSqrRadii.x * vSqrRadii.y <= vSqrRadii.x * vSqrRadii.y * vSqrRadii.z;
	}

	void ClipPointByEllipsoid(Vector3D<Type>& io_vPoint) const
	{
		const Vector3D<Type> vRadii = GetHalfSize();
		const Vector3D<Type> vSqrRadii(sqr(vRadii.x), sqr(vRadii.y), sqr(vRadii.z));
		const Vector3D<Type> vPoint = io_vPoint - GetCenter();

		if (sqr(vPoint.x) * vSqrRadii.y * vSqrRadii.z
			+ sqr(vPoint.y) * vSqrRadii.x * vSqrRadii.z
			+ sqr(vPoint.z) * vSqrRadii.x * vSqrRadii.y > vSqrRadii.x * vSqrRadii.y * vSqrRadii.z)
		{
			Vector3D<Type> vMultipliedPoint(
				IsEqualT(vRadii.x, static_cast<Type>(0), Type(1e-6f)) ? 0 : vPoint.x / vRadii.x,
				IsEqualT(vRadii.y, static_cast<Type>(0), Type(1e-6f)) ? 0 : vPoint.y / vRadii.y,
				IsEqualT(vRadii.z, static_cast<Type>(0), Type(1e-6f)) ? 0 : vPoint.z / vRadii.z);
			vMultipliedPoint.Normalize();
			vMultipliedPoint *= Type(0.999f);
			Vector3D<Type> vClippedPoint(vMultipliedPoint.x * vRadii.x,
				vMultipliedPoint.y * vRadii.y,
				vMultipliedPoint.z * vRadii.z);
			io_vPoint = vClippedPoint + GetCenter();
		}
	}

	void ExtrudePointFromEllipsoid(Vector3D<Type>& io_vPoint) const
	{
		const Vector3D<Type> vRadii = GetHalfSize();
		const Vector3D<Type> vSqrRadii(sqr(vRadii.x), sqr(vRadii.y), sqr(vRadii.z));
		const Vector3D<Type> vPoint = io_vPoint - GetCenter();

		if (sqr(vPoint.x) * vSqrRadii.y * vSqrRadii.z
			+ sqr(vPoint.y) * vSqrRadii.x * vSqrRadii.z
			+ sqr(vPoint.z) * vSqrRadii.x * vSqrRadii.y < vSqrRadii.x * vSqrRadii.y * vSqrRadii.z)
		{
			Vector3D<Type> vMultipliedPoint(
				IsEqualT(vRadii.x, Type(0), Type(1e-6f)) ? 0 : vPoint.x / vRadii.x,
				IsEqualT(vRadii.y, Type(0), Type(1e-6f)) ? 0 : vPoint.y / vRadii.y,
				IsEqualT(vRadii.z, Type(0), Type(1e-6f)) ? 0 : vPoint.z / vRadii.z);
			vMultipliedPoint.Normalize();
			vMultipliedPoint *= 1.001f;
			Vector3D<Type> vClippedPoint(vMultipliedPoint.x * vRadii.x,
				vMultipliedPoint.y * vRadii.y,
				vMultipliedPoint.z * vRadii.z);
			io_vPoint = vClippedPoint + GetCenter();
		}
	}

	void ComputeVerticesLeftHanded(Vector3D<Type> out_vVertex[8]) const
	{
		const Vector3D<Type>& m = vMin;
		const Vector3D<Type>& M = vMax;
		//generate 8 corners of the bbox
		out_vVertex[0] = Vector3D<Type>(m.v[0],m.v[1],m.v[2]); //     7+------+6
		out_vVertex[1] = Vector3D<Type>(M.v[0],m.v[1],m.v[2]); //     /|     /|
		out_vVertex[2] = Vector3D<Type>(M.v[0],M.v[1],m.v[2]); //    / |    / |
		out_vVertex[3] = Vector3D<Type>(m.v[0],M.v[1],m.v[2]); //   / 4+---/--+5
		out_vVertex[4] = Vector3D<Type>(m.v[0],m.v[1],M.v[2]); // 3+------+2 /    y   z
		out_vVertex[5] = Vector3D<Type>(M.v[0],m.v[1],M.v[2]); //  | /    | /     |  /
		out_vVertex[6] = Vector3D<Type>(M.v[0],M.v[1],M.v[2]); //  |/     |/      |/
		out_vVertex[7] = Vector3D<Type>(m.v[0],M.v[1],M.v[2]); // 0+------+1      *---x
	}


	bool IsIntersectingAnotherBox(const CBoundBox<Type>& in_BBox) const
	{
		return !(in_BBox.vMin.x > vMax.x || in_BBox.vMax.x < vMin.x
			  || in_BBox.vMin.y > vMax.y || in_BBox.vMax.y < vMin.y
			  || in_BBox.vMin.z > vMax.z || in_BBox.vMax.z < vMin.z);
	}

	// Пересечение с bbox'ом треугольника. В некоторых ситуациях лишний раз возвращает true.
	// Отнесена в .cpp, поскольку будет подвержена изменениям, а BoundBox у нас в Precompiled.
	bool IsIntersectingTriangleBoxOld(const Vector3D<Type>& in_vPoint0, const Vector3D<Type>& in_vPoint1,
		const Vector3D<Type>& in_vPoint2) const;
	
	// Пересечение с bbox'ом треугольника. В некоторых ситуациях лишний раз возвращает true.
	// Версия с SSE. Когда заработает - заинлайним.
	bool IsIntersectingTriangleBox(const Vector3D<Type>& in_vPoint0, const Vector3D<Type>& in_vPoint1,
								   const Vector3D<Type>& in_vPoint2) const;

	friend inline CBoundBox<Type> operator*(const CBoundBox<Type>& in_BBox, const Type in_fScale)
	{
		return CBoundBox<Type>(
			Type(0.5) * (Type(1) - in_fScale) * in_BBox.vMax + Type(0.5) * (Type(1) + in_fScale) * in_BBox.vMin,
			Type(0.5) * (Type(1) + in_fScale) * in_BBox.vMax + Type(0.5) * (Type(1) - in_fScale) * in_BBox.vMin);
	}

	friend inline bool IsEqualBoundBox(const CBoundBox<Type>& in_BBox1, const CBoundBox<Type>& in_BBox2, Type in_fEpsilon = Type(FLT_MIN) )
	{
		return IsEqualVector3D(in_BBox1.vMax, in_BBox2.vMax, in_fEpsilon)
			&& IsEqualVector3D(in_BBox1.vMin, in_BBox2.vMin, in_fEpsilon);
	}

	bool SubtractBoundBox(const CBoundBox<Type>& in_BBox1,const CBoundBox<Type>& in_BBox2)
	{
		Init();
		if(!in_BBox1.IsIntersectingAnotherBox(in_BBox2))
			return false;
		this->Set(in_BBox1.vMin,in_BBox1.vMax);
		vMin.UpdateMax(in_BBox2.vMin);
		vMax.UpdateMin(in_BBox2.vMax);
		return IsValid();
	}

	void ComputeVertices(Vector3D<Type> out_vVertex[8]) const
	{
		out_vVertex[0] = Vector3D<Type>(m_vMax.x, m_vMax.y, m_vMax.z);
		out_vVertex[1] = Vector3D<Type>(m_vMax.x, m_vMin.y, m_vMax.z);
		out_vVertex[2] = Vector3D<Type>(m_vMin.x, m_vMin.y, m_vMax.z);
		out_vVertex[3] = Vector3D<Type>(m_vMin.x, m_vMax.y, m_vMax.z);

		out_vVertex[4] = Vector3D<Type>(m_vMax.x, m_vMax.y, m_vMin.z);
		out_vVertex[5] = Vector3D<Type>(m_vMax.x, m_vMin.y, m_vMin.z);
		out_vVertex[6] = Vector3D<Type>(m_vMin.x, m_vMin.y, m_vMin.z);
		out_vVertex[7] = Vector3D<Type>(m_vMin.x, m_vMax.y, m_vMin.z);
	}

	void UpdateWithPoints(const vector< Vector3D<Type> >& in_vecPoints);
	
	void Move(const Vector3D<Type>& in_vOffset);

	Type GetVolume()
	{
		if(!IsHasVolume())
			return 0.f;
		Vector3D<Type> vSize = GetSize();
		return vSize.x * vSize.y * vSize.z;
	}

	Type GetMidSize() const
	{
		Vector3D<Type> vSize = GetSize();

		Type a = vSize.x;
		Type b = vSize.y;
		Type c = vSize.z;

		return (a > b && a < c) || (a > c && a < b) ? a : ((b > a && b < c) || (b > c && b < a) ? b : c);
	}
};

template <typename Type>
inline CBoundBox<Type>::CBoundBox() 
{ 
	Init(); 
}

template <typename Type>
inline CBoundBox<Type>::CBoundBox(const Vector3D<Type>& in_vMin, const Vector3D<Type>& in_vMax) :
	vMin(in_vMin),
	vMax(in_vMax)
{
	assert_debug(in_vMin.x <= in_vMax.x && in_vMin.y <= in_vMax.y && in_vMin.z <= in_vMax.z, L"");	
}

template <typename Type>
inline CBoundBox<Type>::CBoundBox(const CBoundBox<Type>& in_BoundBox)
{
	assert_debug(in_BoundBox.IsValid(), L"");
	memcpy(this, &in_BoundBox, sizeof(in_BoundBox));
}

template <typename Type>
inline CBoundBox<Type>::CBoundBox(const Vector3D<Type>& in_vCenter, Type in_fRadius)
{
	Vector3D<Type> vHalf(in_fRadius);
	Set(in_vCenter - vHalf, in_vCenter + vHalf);
}

template <typename Type>
inline CBoundBox<Type>::CBoundBox(const Vector3D<Type>& in_vCenter, Type fHalfX, Type fHalfY, Type fHalfZ)
{
	Vector3D<float> vHalf(fHalfX, fHalfY, fHalfZ);
	Set(in_vCenter - vHalf, in_vCenter + vHalf);
}

template <typename Type>
inline void CBoundBox<Type>::SetCenterHalfSize(const Vector3D<Type>& in_vCenter, const Vector3D<Type>& in_vHalfSize)
{
	Set(in_vCenter - in_vHalfSize, in_vCenter + in_vHalfSize);
}

template <typename Type>
inline void CBoundBox<Type>::Move(const Vector3D<Type>& in_vOffset)
{
	vMax += in_vOffset;
	vMin += in_vOffset;
}