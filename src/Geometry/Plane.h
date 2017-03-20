#pragma once

enum CreatePlane_Type
{
	POINTS3,
	POINTS1_VECTORS2,
	POINTS2_VECTORS1,
};

template <typename Type>
class Plane
{
public:
	Vector3D<Type>	N;
	Type			D;

	inline Plane() {};
	inline Plane(const Vector3D<Type> &_N, Type _D) { N=_N; D=_D; }
	inline Plane(const Vector3D<Type>& _1, const Vector3D<Type>& _2, const Vector3D<Type>& _3, CreatePlane_Type type)
	{ Create( _1, _2, _3, type ); }

	void Create( const Vector3D<Type>& _1, const Vector3D<Type>& _2, const Vector3D<Type>& _3, CreatePlane_Type type )
	{
		switch(type)
		{
		case POINTS3:
			N = CrossProduct( (_2-_1), (_3-_1) ).Normalize();
			break;
		case POINTS1_VECTORS2:
			N = CrossProduct(_2, _3).Normalize();
			break;
		case POINTS2_VECTORS1:
			N = CrossProduct( (_2-_1), _3 ).Normalize();
			break;
		default:
			__assume(0);
			assert_debug(false, L"");
		}

		D = DotProduct(N, _1);
	}

	inline Plane&	operator = ( const Plane& p ) { N=p.N; D=p.D; return *this; }
	inline Plane	operator - () const { return Plane(-N,-D); }

	void Normalize()
	{
		float mag;
		mag = sqrt(N.v[0]*N.v[0] + N.v[1]*N.v[1] + N.v[2]*N.v[2]);
		N.v[0] = N.v[0] / mag;
		N.v[1] = N.v[1] / mag;
		N.v[2] = N.v[2] / mag;
		D = D / mag;
	}
};


struct SFOV_Tan
{
public:

	SFOV_Tan() : m_fFOVAngle(0), m_fTanFOVAngle(0)
	{}

	SFOV_Tan(float in_fValueDegrees)
	{
		SetFOVAngle(in_fValueDegrees);
	}

	void SetFOVAngle(float in_fValueDergees)
	{
		assert_debug(in_fValueDergees < 180, L"illegal FOV angle");
		m_fFOVAngle = in_fValueDergees;
		m_fTanFOVAngle = tanf(0.5f * DEG2RAD * m_fFOVAngle);
	}

	__forceinline float GetFOVAngle() const { return m_fFOVAngle; }
	__forceinline float GetTanFOVAngle() const { return m_fTanFOVAngle; }

private:
	float	m_fFOVAngle;
	float	m_fTanFOVAngle;
};


template<typename TYPE>
struct CFrustum
{
	CFrustum()
	{
		for(int i = 0; i < 6; i++)
		{
			Planes[i].N.zero();
			Planes[i].D = 0;
		}
	}

	CFrustum(float in_fNearPlane, float in_fFarPlane, float in_fTanW, float in_fTanH, const Matrix3x3<TYPE>& in_mOrientation,
		const Vector3D<TYPE>& in_vPlace)
	{
		float half_w = in_fFarPlane * in_fTanW;
		float half_h = in_fFarPlane * in_fTanH;

		Vector3D<float> cen = in_vPlace + in_fFarPlane * in_mOrientation.forward();
		Vector3D<float> tl = cen + in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
		Vector3D<float> tr = cen - in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
		Vector3D<float> bl = cen + in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;
		Vector3D<float> br = cen - in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;

		Planes[0].Create( in_vPlace, tl, bl, POINTS3 );	// left
		Planes[1].Create( in_vPlace, br, tr, POINTS3 );	// right
		Planes[2].Create( in_vPlace + in_mOrientation.forward() * in_fFarPlane, 
			in_mOrientation.up(), TYPE(-1) * in_mOrientation.left(), POINTS1_VECTORS2 ); // far
		Planes[3].Create( in_vPlace, bl, br, POINTS3 );	// low
		Planes[4].Create( in_vPlace, tr, tl, POINTS3 );	// high

		Planes[5].Create( in_vPlace + in_mOrientation.forward() * in_fNearPlane, 
			 TYPE(-1) * in_mOrientation.left(), in_mOrientation.up(), POINTS1_VECTORS2 ); // near
	}

	CFrustum(const Matrix4x4<TYPE>& comboMatrix)
	{
		// Left clipping plane
		Planes[0].N.v[0] = comboMatrix._14 + comboMatrix._11;
		Planes[0].N.v[1] = comboMatrix._24 + comboMatrix._21;
		Planes[0].N.v[2] = comboMatrix._34 + comboMatrix._31;
		Planes[0].D = comboMatrix._44 + comboMatrix._41;

		// Right clipping plane
		Planes[1].N.v[0] = comboMatrix._14 - comboMatrix._11;
		Planes[1].N.v[1] = comboMatrix._24 - comboMatrix._21;
		Planes[1].N.v[2] = comboMatrix._34 - comboMatrix._31;
		Planes[1].D = comboMatrix._44 - comboMatrix._41;

		// Top clipping plane
		Planes[2].N.v[0] = comboMatrix._14 - comboMatrix._12;
		Planes[2].N.v[1] = comboMatrix._24 - comboMatrix._22;
		Planes[2].N.v[2] = comboMatrix._34 - comboMatrix._32;
		Planes[2].D = comboMatrix._44 - comboMatrix._42;

		// Bottom clipping plane
		Planes[3].N.v[0] = comboMatrix._14 + comboMatrix._12;
		Planes[3].N.v[1] = comboMatrix._24 + comboMatrix._22;
		Planes[3].N.v[2] = comboMatrix._34 + comboMatrix._32;
		Planes[3].D = comboMatrix._44 + comboMatrix._42;

		// Near clipping plane
		Planes[4].N.v[0] = comboMatrix._13;
		Planes[4].N.v[1] = comboMatrix._23;
		Planes[4].N.v[2] = comboMatrix._33;
		Planes[4].D = comboMatrix._43;

		// Far clipping plane
		Planes[5].N.v[0] = comboMatrix._14 - comboMatrix._13;
		Planes[5].N.v[1] = comboMatrix._24 - comboMatrix._23;
		Planes[5].N.v[2] = comboMatrix._34 - comboMatrix._33;
		Planes[5].D = comboMatrix._44 - comboMatrix._43;
		
		// Normalize the plane equations, if requested
		Planes[0].Normalize();
		Planes[1].Normalize();
		Planes[2].Normalize();
		Planes[3].Normalize();
		Planes[4].Normalize();
		Planes[5].Normalize();
		
	}

	/*CFrustum(const Vector3D<TYPE>& in_vCenter, const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vHalfSizes)
	{
		Vector3D<float> vPoints[8];

		Box3<float> OBB(in_vCenter, in_mOrientation, in_vHalfSizes);

		OBB.ComputeVertices(vPoints);

		Planes[0].Create(vPoints[2], vPoints[6], vPoints[3], POINTS3);
		Planes[1].Create(vPoints[0], vPoints[1], vPoints[4], POINTS3);
		Planes[2].Create(vPoints[1], vPoints[3], vPoints[5], POINTS3);
		Planes[3].Create(vPoints[0], vPoints[2], vPoints[1], POINTS3);
		Planes[4].Create(vPoints[4], vPoints[5], vPoints[6], POINTS3);
		Planes[5].Create(vPoints[0], vPoints[4], vPoints[2], POINTS3);
	}*/

	Plane<TYPE> Planes[6];

	Plane<TYPE>& operator []( size_t id ) 
	{		
		assert_debug(id < 6, L"");
		return Planes[id];
	}

	const Plane<TYPE>& operator []( size_t id ) const 
	{		
		assert_debug(id < 6, L"");
		return Planes[id];
	}
};