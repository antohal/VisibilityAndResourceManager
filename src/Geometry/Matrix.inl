
//------------------------------------------------------------------------------------------------------------------//
//******************************************************************************************************************//
//------------------------------------------------------------------------------------------------------------------//
template <typename TYPE>
Matrix2x2<TYPE> &Matrix2x2<TYPE> :: operator *=(const float f)
{
	m[0][0]*=f;	m[0][1]*=f;
	m[1][0]*=f;	m[1][1]*=f;
	return *this;
}

template <typename TYPE>
void Matrix2x2<TYPE>::Rotate(float angle)
{
	TYPE cosine;
	TYPE sine;
	SinCos(angle, sine, cosine);

	m[0][0] = cosine;
	m[1][1] = cosine;
	m[1][0] = -sine;
	m[0][1] = sine;
}


#ifdef _DEBUG
template <typename TYPE>
void Matrix2x2<TYPE>::AssertOrthogonal() const
{
	assert_narek_ex( fabs( Length(line1) - static_cast<TYPE>(1) ) < static_cast<TYPE>(0.001), L"Assert Orthogonal");
	assert_narek_ex( fabs( Length(line2) - static_cast<TYPE>(1) ) < static_cast<TYPE>(0.001), L"Assert Orthogonal");
	assert_narek( fabs(DotProduct(line1, line2)) < static_cast<TYPE>(0.001) );
}
#endif


template <typename TYPE>
inline Vector2D<TYPE> mul(const Vector2D<TYPE> &u,const Matrix2x2<TYPE>& m)
{
	Vector2D<TYPE> res;
	res.x = u.x * m._11 + u.y * m._21;
	res.y = u.x * m._12 + u.y * m._22;

	return res;
}


template <typename TYPE>
Matrix2x2<TYPE> mul(const Matrix2x2<TYPE> &v,const Matrix2x2<TYPE> &u)
{
	Matrix2x2<TYPE> r;
	r[0][0] = v.m[0][0]*u.m[0][0] + v.m[0][1]*u.m[1][0];
	r[0][1] = v.m[0][0]*u.m[0][1] + v.m[0][1]*u.m[1][1];
	r[1][0] = v.m[1][0]*u.m[0][0] + v.m[1][1]*u.m[1][0];
	r[1][1] = v.m[1][0]*u.m[0][1] + v.m[1][1]*u.m[1][1];
	return r;
}

template <typename TYPE>
inline Vector2D<TYPE> mul(const Matrix2x2<TYPE>& m, const Vector2D<TYPE> &u)
{
	Vector2D<TYPE> res;
	res.x = u.x * m._11 + u.y * m._12;
	res.y = u.x * m._21 + u.y * m._22;

	return res;
}

template <typename TYPE>
inline Vector3D<TYPE> mul( const Vector3D<TYPE>& u, const Matrix3x3<TYPE>& m )
{
	Vector3D<TYPE> res;

	res.x = u.x * m._11 + u.y * m._21 + u.z * m._31;
	res.y = u.x * m._12 + u.y * m._22 + u.z * m._32;
	res.z = u.x * m._13 + u.y * m._23 + u.z * m._33;

	return res;
}

template <typename TYPE>
inline Vector4D<TYPE> mul(const Vector4D<TYPE>& u, const Matrix4x4<TYPE>& m)
{
	Vector4D<TYPE> res;

	res.x = u.x * m._11 + u.y * m._21 + u.z * m._31 + u.w * m._41;
	res.y = u.x * m._12 + u.y * m._22 + u.z * m._32 + u.w * m._42;
	res.z = u.x * m._13 + u.y * m._23 + u.z * m._33 + u.w * m._43;
	res.w = u.x * m._14 + u.y * m._24 + u.z * m._34 + u.w * m._44;

	return res;
}

template <typename TYPE>
inline Vector3D<TYPE> mul_transposed( const Vector3D<TYPE>& u, const Matrix3x3<TYPE>& m )
{
	Vector3D<TYPE> res;

	res.x = u.x * m._11 + u.y * m._12 + u.z * m._13;
	res.y = u.x * m._21 + u.y * m._22 + u.z * m._23;
	res.z = u.x * m._31 + u.y * m._32 + u.z * m._33;

	return res;
}

//------------------------------------------------------------------------------------------------------------------//
//******************************************************************************************************************//
//------------------------------------------------------------------------------------------------------------------//
template <typename TYPE>
void Matrix4x4<TYPE>::Transpose( const Matrix4x4 &in )
{
	_11 = in._11;
	_12 = in._21;
	_13 = in._31;
	_14 = in._41;

	_21 = in._12;
	_22 = in._22;
	_23 = in._32;
	_24 = in._42;

	_31 = in._13;
	_32 = in._23;
	_33 = in._33;
	_34 = in._43;

	_41 = in._14;
	_42 = in._24;
	_43 = in._34;
	_44 = in._44;
}

template <typename TYPE>
Matrix4x4<TYPE> operator *(const Matrix4x4<TYPE> &v,const Matrix4x4<TYPE> &u)
{
	return mul<TYPE>( v, u );
}

template <typename TYPE>
inline Matrix4x4<TYPE> &Matrix4x4<TYPE>::operator *=(const TYPE f)
{
	m[0][0]*=f;	m[0][1]*=f;	m[0][2]*=f;	m[0][3]*=f;
	m[1][0]*=f;	m[1][1]*=f;	m[1][2]*=f;	m[1][3]*=f;
	m[2][0]*=f;	m[2][1]*=f;	m[2][2]*=f;	m[2][3]*=f;
	m[3][0]*=f;	m[3][1]*=f;	m[3][2]*=f;	m[3][3]*=f;

	return *this;
}


template <typename TYPE>
inline Vector3D<TYPE> mul( const Matrix3x3<TYPE>& m, const Vector3D<TYPE>& u )
{
	Vector3D<TYPE> res;
	res.x = u.x * m._11 + u.y * m._12 + u.z * m._13;
	res.y = u.x * m._21 + u.y * m._22 + u.z * m._23;
	res.z = u.x * m._31 + u.y * m._32 + u.z * m._33;
	return res;
}

template <typename TYPE>
inline Vector4D<TYPE> mul(const Matrix4x4<TYPE>& m, const Vector4D<TYPE>& u)
{
	Vector4D<TYPE> res;
	res.x = u.x * m._11 + u.y * m._12 + u.z * m._13 + u.w * m._14;
	res.y = u.x * m._21 + u.y * m._22 + u.z * m._23 + u.w * m._24;
	res.z = u.x * m._31 + u.y * m._32 + u.z * m._33 + u.w * m._34;
	res.w = u.x * m._41 + u.y * m._42 + u.z * m._43 + u.w * m._44;

	return res;
}

template <typename TYPE>
inline Matrix3x3<TYPE> mul(const Matrix3x3<TYPE> &v,const Matrix3x3<TYPE> &u)
{
	Matrix3x3<TYPE> r;
	//136 тактов вместо 161
	r[0][0] = v.m[0][0]*u.m[0][0] + v.m[0][1]*u.m[1][0] + v.m[0][2]*u.m[2][0];
	r[0][1] = v.m[0][0]*u.m[0][1] + v.m[0][1]*u.m[1][1] + v.m[0][2]*u.m[2][1];
	r[0][2] = v.m[0][0]*u.m[0][2] + v.m[0][1]*u.m[1][2] + v.m[0][2]*u.m[2][2];
	r[1][0] = v.m[1][0]*u.m[0][0] + v.m[1][1]*u.m[1][0] + v.m[1][2]*u.m[2][0];
	r[1][1] = v.m[1][0]*u.m[0][1] + v.m[1][1]*u.m[1][1] + v.m[1][2]*u.m[2][1];
	r[1][2] = v.m[1][0]*u.m[0][2] + v.m[1][1]*u.m[1][2] + v.m[1][2]*u.m[2][2];
	r[2][0] = v.m[2][0]*u.m[0][0] + v.m[2][1]*u.m[1][0] + v.m[2][2]*u.m[2][0];
	r[2][1] = v.m[2][0]*u.m[0][1] + v.m[2][1]*u.m[1][1] + v.m[2][2]*u.m[2][1];
	r[2][2] = v.m[2][0]*u.m[0][2] + v.m[2][1]*u.m[1][2] + v.m[2][2]*u.m[2][2];
	return r;
}

template <typename TYPE>
Matrix2x2<TYPE> mul_transposed(const Matrix2x2<TYPE> &u,const Matrix2x2<TYPE> &v)
{
	Matrix2x2<TYPE> r;
	r[0][0] = v.m[0][0]*u.m[0][0] + v.m[0][1]*u.m[0][1];
	r[0][1] = v.m[0][0]*u.m[1][0] + v.m[0][1]*u.m[1][1];
	r[1][0] = v.m[1][0]*u.m[0][0] + v.m[1][1]*u.m[0][1];
	r[1][1] = v.m[1][0]*u.m[1][0] + v.m[1][1]*u.m[1][1];
	return r;
}

template <typename TYPE>
inline Matrix3x3<TYPE> mul_transposed(const Matrix3x3<TYPE> &v,const Matrix3x3<TYPE> &u)
{
	Matrix3x3<TYPE> r;
	r[0][0] = v.m[0][0]*u.m[0][0] + v.m[0][1]*u.m[0][1] + v.m[0][2]*u.m[0][2];
	r[0][1] = v.m[0][0]*u.m[1][0] + v.m[0][1]*u.m[1][1] + v.m[0][2]*u.m[1][2];
	r[0][2] = v.m[0][0]*u.m[2][0] + v.m[0][1]*u.m[2][1] + v.m[0][2]*u.m[2][2];
	r[1][0] = v.m[1][0]*u.m[0][0] + v.m[1][1]*u.m[0][1] + v.m[1][2]*u.m[0][2];
	r[1][1] = v.m[1][0]*u.m[1][0] + v.m[1][1]*u.m[1][1] + v.m[1][2]*u.m[1][2];
	r[1][2] = v.m[1][0]*u.m[2][0] + v.m[1][1]*u.m[2][1] + v.m[1][2]*u.m[2][2];
	r[2][0] = v.m[2][0]*u.m[0][0] + v.m[2][1]*u.m[0][1] + v.m[2][2]*u.m[0][2];
	r[2][1] = v.m[2][0]*u.m[1][0] + v.m[2][1]*u.m[1][1] + v.m[2][2]*u.m[1][2];
	r[2][2] = v.m[2][0]*u.m[2][0] + v.m[2][1]*u.m[2][1] + v.m[2][2]*u.m[2][2];
	return r;
}

template <typename TYPE>
inline Matrix4x4<TYPE> mul_transposed(const Matrix4x4<TYPE> &v,const Matrix4x4<TYPE> &u)
{
	Matrix4x4<TYPE> r;
	ZeroMemory(&r, sizeof(r));

	for(unsigned i = 0; i < 4; i++)
		for(unsigned j = 0; j < 4; j++)
			for(unsigned k = 0; k < 4; k++)
				r[i][j] += v.m[i][k] * u.m[j][k];
	return r;
}


template <typename TYPE>
inline Matrix4x4<TYPE> mul( const Matrix4x4<TYPE>& v, const Matrix4x4<TYPE>& u )
{
#ifdef SSE

		Matrix4x4<TYPE> r;

		__m128 A11, A12, A13, A14;
		__m128 A21, A22, A23, A24;
		__m128 A31, A32, A33, A34;
		__m128 A41, A42, A43, A44;

		A11 = _mm_load_ps1(&v._11);
		A12 = _mm_load_ps1(&v._12);
		A13 = _mm_load_ps1(&v._13);
		A14 = _mm_load_ps1(&v._14);

		A21 = _mm_load_ps1(&v._21);
		A22 = _mm_load_ps1(&v._22);
		A23 = _mm_load_ps1(&v._23);
		A24 = _mm_load_ps1(&v._24);

		A31 = _mm_load_ps1(&v._31);
		A32 = _mm_load_ps1(&v._32);
		A33 = _mm_load_ps1(&v._33);
		A34 = _mm_load_ps1(&v._34);

		A41 = _mm_load_ps1(&v._41);
		A42 = _mm_load_ps1(&v._42);
		A43 = _mm_load_ps1(&v._43);
		A44 = _mm_load_ps1(&v._44);

		__m128 B1, B2, B3, B4;
		B1 = _mm_loadu_ps((float*)&u.line1);
		B2 = _mm_loadu_ps((float*)&u.line2);
		B3 = _mm_loadu_ps((float*)&u.line3);
		B4 = _mm_loadu_ps((float*)&u.line4);

		__m128 Line, Mul;

		Line = _mm_mul_ps(A11, B1);
		Mul = _mm_mul_ps(A12, B2);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A13, B3);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A14, B4);
		Line = _mm_add_ps(Line, Mul);
		_mm_storeu_ps((float*)&r.line1, Line);

		Line = _mm_mul_ps(A21, B1);
		Mul = _mm_mul_ps(A22, B2);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A23, B3);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A24, B4);
		Line = _mm_add_ps(Line, Mul);
		_mm_storeu_ps((float*)&r.line2, Line);

		Line = _mm_mul_ps(A31, B1);
		Mul = _mm_mul_ps(A32, B2);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A33, B3);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A34, B4);
		Line = _mm_add_ps(Line, Mul);
		_mm_storeu_ps((float*)&r.line3, Line);

		Line = _mm_mul_ps(A41, B1);
		Mul = _mm_mul_ps(A42, B2);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A43, B3);
		Line = _mm_add_ps(Line, Mul);
		Mul = _mm_mul_ps(A44, B4);
		Line = _mm_add_ps(Line, Mul);
		_mm_storeu_ps((float*)&r.line4, Line);

		return r;

#else	

		Matrix4x4<TYPE> r;
		int i,j;

		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				r.m[i][j]=v.m[i][0]*u.m[0][j]+
				v.m[i][1]*u.m[1][j]+
				v.m[i][2]*u.m[2][j]+
				v.m[i][3]*u.m[3][j];
		return r;

#endif //SSE
}

template <typename TYPE>
inline Matrix4x4<TYPE> mul_inversed(const Matrix4x4<TYPE> &v,const Matrix4x4<TYPE> &u)
{
	Matrix4x4<TYPE> i;
	i.InverseTransform(u);
	return mul(v, i);
}


template <typename TYPE>
void Matrix4x4<TYPE>::RotateX( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_11 = 1.f;
	_22 = _33 = cosine;
	_23 = sine;
	_32 = -sine;
	_44 = 1.f;
}

template <typename TYPE>
void Matrix4x4<TYPE>::RotateY( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_22 = 1.f;
	_11 = _33 = cosine;
	_31 = sine;
	_13 = -sine;
	_44 = 1.f;
}

template <typename TYPE>
void Matrix4x4<TYPE>::RotateZ( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_33 = 1.f;
	_11 = _22 = cosine;
	_12 = sine;
	_21 = -sine;
	_44 = 1.f;
}

template <typename TYPE>
inline void Matrix4x4<TYPE>::SetPosition( const Vector3D<TYPE>& in_vPosition ) 
{
	line4 = in_vPosition;
}

template <typename TYPE>
void Matrix4x4<TYPE>::InverseTransform(const Matrix4x4<TYPE> &in)
{
	_11 = in._11;
	_12 = in._21;
	_13 = in._31;
	_14 = 0;

	_21 = in._12;
	_22 = in._22;
	_23 = in._32;
	_24 = 0;

	_31 = in._13;
	_32 = in._23;
	_33 = in._33;
	_34 = 0;

	_41 = - dot(in.line4, in.line1);
	_42 = - dot(in.line4, in.line2);
	_43 = - dot(in.line4, in.line3);
	_44 = 1;
}

template <typename TYPE>
const Matrix4x4<TYPE>& Matrix4x4<TYPE>::InverseTransform()
{
	Matrix4x4<TYPE> mTmp = *this;
	_11 = mTmp._11;
	_12 = mTmp._21;
	_13 = mTmp._31;
	_14 = 0;

	_21 = mTmp._12;
	_22 = mTmp._22;
	_23 = mTmp._32;
	_24 = 0;

	_31 = mTmp._13;
	_32 = mTmp._23;
	_33 = mTmp._33;
	_34 = 0;

	_41 = - dot(mTmp.line4, mTmp.line1);
	_42 = - dot(mTmp.line4, mTmp.line2);
	_43 = - dot(mTmp.line4, mTmp.line3);
	_44 = 1;
	return *this;
}


template <typename TYPE>
void Matrix3x3<TYPE>::RotateX( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_11 = 1.f;
	_22 = _33 = cosine;
	_23 = sine;
	_32 = -sine;
}

template <typename TYPE>
void Matrix3x3<TYPE>::RotateY( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_22 = 1.f;
	_11 = _33 = cosine;
	_31 = sine;
	_13 = -sine;
}

template <typename TYPE>
void Matrix3x3<TYPE>::RotateZ( float fAngle )
{
	TYPE cosine;
	TYPE sine;

	SinCos( fAngle, sine, cosine );

	zero();

	_33 = 1.f;
	_11 = _22 = cosine;
	_12 = sine;
	_21 = -sine;
}

template <typename TYPE>
void Matrix3x3<TYPE>::RotateXYZ(float angle_x,float angle_y,float angle_z)
{
	TYPE c0, s0,  c1, s1,  c2, s2;
	SinCos(angle_x, s0, c0);
	SinCos(angle_y, s1, c1);
	SinCos(angle_z, s2, c2);

	m[0][0]=c1*c2;	m[1][0]=s0*s1*c2-c0*s2;	m[2][0]=c0*s1*c2+s0*s2;
	m[0][1]=c1*s2;	m[1][1]=c0*c2+s0*s1*s2;	m[2][1]=c0*s1*s2-s0*c2;
	m[0][2]=-s1;	m[1][2]=s0*c1;			m[2][2]=c0*c1;		
}

template <typename TYPE>
void Matrix3x3<TYPE>::RotateAroundVector (const Vector3D<TYPE>& v, const TYPE a)
{
	TYPE cosine, sine;
	SinCos(a, sine, cosine);

	_11 = cosine + (1 - cosine)*v.x*v.x;	_12 = (1 - cosine)*v.x*v.y - sine*v.z;	_13 = (1 - cosine)*v.x*v.z + sine*v.y;
	_21 = (1 - cosine)*v.y*v.x + sine*v.z;	_22 = cosine + (1 - cosine)*v.y*v.y;	_23 = (1 - cosine)*v.y*v.z - sine*v.x;
	_31 = (1 - cosine)*v.z*v.x - sine*v.y;	_32 = (1 - cosine)*v.z*v.y + sine*v.x;	_33 = cosine + (1 - cosine)*v.z*v.z;
}

//////////////////////////////////////////

template<typename TYPE>
inline Matrix3x3<TYPE> GetMatrixFromUpDirection(const Vector3D<TYPE>& in_vPreliminaryForward, 
	const Vector3D<TYPE>& in_vFinalUp)
{
	assert_debug(IsEqualT<TYPE>(in_vFinalUp.Length(), TYPE(1) ), L"");

	Vector3D<TYPE> vWorldLeft = CrossProduct(in_vFinalUp, in_vPreliminaryForward);
	TYPE fWorldLeft = vWorldLeft.NormalizeL();
	if( IsEqualT<TYPE>( fWorldLeft, TYPE(1e-6) ) )
		vWorldLeft = Perpendicular(in_vFinalUp);

	Vector3D<TYPE> vWorldForward = CrossProduct(vWorldLeft, in_vFinalUp);
	Matrix3x3<TYPE> mWorldUp;
	mWorldUp.SetBasis(vWorldForward, vWorldLeft, in_vFinalUp);
	return mWorldUp;
}

template<typename TYPE>
inline Matrix3x3<TYPE> GetMatrixFromForwardDirection(const Vector3D<TYPE>& in_vFinalForward, 
	const Vector3D<TYPE>& in_vPreliminaryUp)
{
	assert_debug( IsEqualT<TYPE>( in_vFinalForward.Length(), TYPE(1) ), L"" );

	Vector3D<TYPE> vWorldLeft = CrossProduct(in_vPreliminaryUp, in_vFinalForward);
	TYPE fWorldLeft = vWorldLeft.NormalizeL();
	if( IsEqualT( fWorldLeft, TYPE(1e-6) ) )
		vWorldLeft = Perpendicular(in_vFinalForward);

	Vector3D<TYPE> vWorldUp = CrossProduct(in_vFinalForward, vWorldLeft);
	Matrix3x3<TYPE> mWorldUp;
	mWorldUp.SetBasis(in_vFinalForward, vWorldLeft, vWorldUp);
	return mWorldUp;
}
