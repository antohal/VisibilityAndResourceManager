#pragma once

#include "IndependenceGeometryFunctions.h"

template <typename TYPE> class Matrix4x4;
template <typename TYPE> class Matrix3x3;

template <typename TYPE>
class Matrix2x2
{
public:
	union 
	{
		struct 
		{
			TYPE        _11, _12;
			TYPE        _21, _22;
		};

		TYPE m[2][2];

		struct
		{
			Vector2D<TYPE> line1;
			Vector2D<TYPE> line2;
		};
	};

	inline Matrix2x2 () {}
	inline Matrix2x2<TYPE> (const Matrix3x3<TYPE>& m3x3)
	{
		m[0][0] = m3x3.m[0][0];
		m[0][1] = m3x3.m[0][1];
		m[1][0] = m3x3.m[1][0];
		m[1][1] = m3x3.m[1][1];
	}
	inline Matrix2x2<TYPE> (const Matrix2x2<TYPE>& m2x2)
	{
		m[0][0] = m2x2.m[0][0];
		m[0][1] = m2x2.m[0][1];
		m[1][0] = m2x2.m[1][0];
		m[1][1] = m2x2.m[1][1];
	}

	inline Matrix2x2<TYPE>& operator = ( const Matrix2x2<TYPE>& _m ) 
	{
		m[0][0] = _m.m[0][0];
		m[0][1] = _m.m[0][1];
		m[1][0] = _m.m[1][0];
		m[1][1] = _m.m[1][1];
		return *this;
	}

	inline float* operator [](int id) {return m[id];}


	inline int operator == (const Matrix2x2<TYPE> &v) 
	{
		return (
				(m[0][0]==v.m[0][0])&&(m[0][1]==v.m[0][1])&&
				(m[1][0]==v.m[1][0])&&(m[1][1]==v.m[1][1])
				);
	}


	inline void		zero() {memset(m,0,2*2*sizeof(TYPE));}
	inline void		one() {m[0][0]=m[1][1]=1.f; m[0][1]=m[1][0]=0.f;}
	void			Rotate(float angle);

	Matrix2x2<TYPE> &operator *= (const float f);

	//установка матрицы по базису
	inline void SetBasis( const Vector2D<TYPE> &forward,const Vector2D<TYPE> &up )
	{
		line1 = forward;
		line2 = up;
		AssertOrthogonal();
	}

	inline void SetBasis( const Vector2D<TYPE> &forward )
	{
		line1 = forward;
		line1.Normalize();

		line2 = Vector2D<TYPE>(line1.y, -line1.x); //против часовой
		AssertOrthogonal();
	}
	
	

#ifdef _DEBUG
	void AssertOrthogonal() const;
#else // !ASSERTS
	inline void AssertOrthogonal() const {}
#endif // ASSERTS
};




template <typename TYPE>
class Matrix3x3
{
public:
	union 
	{
		struct 
		{
			TYPE        _11, _12, _13;
			TYPE        _21, _22, _23;
			TYPE        _31, _32, _33;
		};

		TYPE m[3][3];

		struct
		{
			Vector3D<TYPE> line1;
			Vector3D<TYPE> line2;
			Vector3D<TYPE> line3;
		};

		struct
		{
			Vector3D<TYPE> lines[3];
		};

		struct
		{
			TYPE a[9];
		};
	};

	inline Matrix3x3 () {}
	inline Matrix3x3( const Vector3D<TYPE> &front,
		const Vector3D<TYPE> &left,
		const Vector3D<TYPE> &up )
	{
		line1 = front;
		line2 = left;
		line3 = up;
		AssertOrthogonal();
	}

	Matrix3x3( const Matrix3x3 &_m )
	{ 
		memcpy(m, _m.m, 3*3*sizeof(TYPE)); 
	}

	template<typename T2> explicit Matrix3x3(const Matrix3x3<T2>& src)
	{ 
		line1 = static_cast<Vector3D<TYPE> >(src.line1);
		line2 = static_cast<Vector3D<TYPE> >(src.line2);
		line3 = static_cast<Vector3D<TYPE> >(src.line3);
	}

	Matrix3x3( const Matrix4x4<TYPE> &_m )
	{ 
		line1 = _m.line1;
		line2 = _m.line2;
		line3 = _m.line3;
	}


	Matrix3x3<TYPE>& operator = ( const Matrix3x3<TYPE>& _m ) 
	{ 
		memcpy(m, _m.m, 3*3*sizeof(TYPE)); return *this; 
	}

	Matrix3x3<TYPE>& operator = ( const Matrix4x4<TYPE>& _m ) 
	{ 
		line1 = _m.line1;
		line2 = _m.line2;
		line3 = _m.line3;
		return *this;
	}

	
	void operator *= (const TYPE& Value) 
	{ 
		line1 *= Value;
		line2 *= Value;
		line3 *= Value;
	}

	TYPE* operator [](int id) {return m[id];}
	const TYPE* operator [](int id) const {return m[id];}

	int operator ==(const Matrix3x3<TYPE> &v) {return (
		(m[0][0]==v.m[0][0])&&(m[0][1]==v.m[0][1])&&(m[0][2]==v.m[0][2])&&
		(m[1][0]==v.m[1][0])&&(m[1][1]==v.m[1][1])&&(m[1][2]==v.m[1][2])&&
		(m[2][0]==v.m[2][0])&&(m[2][1]==v.m[2][1])&&(m[2][2]==v.m[2][2])
		);}

	const Matrix3x3& operator += (const Matrix3x3& M)
	{
		for(int i = 0; i < 3; ++i)
			for(int j = 0; j < 3; ++j)
				m[i][j] += M.m[i][j];
		return *this;
	}

	void zero() {memset(m, 0, 3*3*sizeof(TYPE));}
	void one() {zero();	m[0][0]=1.f;	m[1][1]=1.f;	m[2][2]=1.f;}
	bool check_one() const {return (m[0][0]==1.f) &&	(m[1][1]==1.f) &&	(m[2][2]==1.f);}

	//Строки матрицы (базис!)
	const Vector3D<TYPE> &x() const { return line1; }
	const Vector3D<TYPE> &y() const { return line2; }
	const Vector3D<TYPE> &z() const { return line3; }

	const Vector3D<TYPE> &forward() const { return line1; }
	const Vector3D<TYPE> &left() const { return line2; }
	const Vector3D<TYPE> &up() const { return line3; }

	//КРИВОЙ Базис матрицы - столбцы
	Vector3D<TYPE> kx() const { return (Vector3D<TYPE>(m[0][0], m[1][0], m[2][0])); };
	Vector3D<TYPE> ky() const { return (Vector3D<TYPE>(m[0][1], m[1][1], m[2][1])); };
	Vector3D<TYPE> kz() const { return (Vector3D<TYPE>(m[0][2], m[1][2], m[2][2])); };

	inline void RotateX( float angle );
	inline void RotateY( float angle );
	inline void RotateZ( float angle );
	inline void RotateXYZ(float angle_x,float angle_y,float angle_z);
	inline void RotateAroundVector (const Vector3D<TYPE>& v, const TYPE a);

	inline void SetBasis(	const Vector3D<TYPE> &forward,  //установка матрицы по базису
							const Vector3D<TYPE> &left, 
							const Vector3D<TYPE> &up, bool in_bCheckOrthogonal = true )
	{
		line1 = forward;
		line2 = left;
		line3 = up;
#ifdef _DEBUG
		if( in_bCheckOrthogonal )
			AssertOrthogonal();
#endif//def _DEBUG
	}

	 //корректировка ортонормированности для особо ответственных случаев
	inline void Orthonormalize()
	{
		Vector3D<TYPE> mx = x();
		Vector3D<TYPE> my = y();
		Vector3D<TYPE> mz = z();
		mz = Normalize( cross(mx, my ) );
		my = Normalize( cross(mz, mx ) );
		mx = Normalize( cross(my, mz ) );
		SetBasis(mx, my, mz);
	}

	void Transpose(const Matrix3x3<TYPE>& in)
	{
		_11 = in._11;
		_12 = in._21;
		_13 = in._31;

		_21 = in._12;
		_22 = in._22;
		_23 = in._32;

		_31 = in._13;
		_32 = in._23;
		_33 = in._33;
	}

	Vector3D<TYPE> GetAngles() const
	{
		Vector3D<TYPE> NewAA;
		if( TAbs<TYPE>( m[0][2])>1.f - 1e-6f )
		{
			NewAA.y = -m[0][2]*PI_BY_2;
			NewAA.x = 0.f;
			TYPE fFloat0 = m[1][1];
			CLIP_NUMBER(fFloat0, -1.f, 1.f);
			NewAA.z=-sign(m[1][0])*acosf(fFloat0);
		}
		else
		{
			TYPE fFloat0 = m[0][2];
			CLIP_NUMBER(fFloat0, -1.f, 1.f);
			NewAA.y=-asinf(fFloat0);
			int i=sign(m[2][2]), k=sign(m[0][0]);
			if (i==0) i=1; if (k==0) k=1;
			TYPE Float1=m[1][2]/TYPE(cosf(NewAA.y));
			if (Float1>1.f) Float1=1.f; if (Float1<-1.f) Float1=-1.f;
			NewAA.x=PI*TYPE(i==-1)+i*asinf(Float1);
			TYPE Float2=m[0][1]/TYPE(cosf(NewAA.y));
			if (Float2>1.f) Float2=1.f; if (Float2<-1.f) Float2=-1.f;
			NewAA.z=PI*TYPE(k==-1)+k*TYPE(asinf(Float2));
		}
		return NewAA;
	}

	void Transpose()
	{
		swap(_12, _21);
		swap(_13, _31);
		swap(_23, _32);
	}

	static Matrix3x3<TYPE> Identity()
	{
		Matrix3x3<TYPE> m;
		m._11 = TYPE(1); m._12 = TYPE(0); m._13 = TYPE(0);
		m._21 = TYPE(0); m._22 = TYPE(1); m._23 = TYPE(0);
		m._31 = TYPE(0); m._32 = TYPE(0); m._33 = TYPE(1);
		return m;
	}

#ifdef _DEBUG
	void AssertOrthogonal() const
	{
		assert_debug( fabs( Length(line1) - static_cast<TYPE>(1) ) < static_cast<TYPE>(0.001), L"AssertOrthogonal(): !line1 = %g." );
		assert_debug( fabs( Length(line2) - static_cast<TYPE>(1) ) < static_cast<TYPE>(0.001), L"AssertOrthogonal(): !line2 = %g." );
		assert_debug( fabs( Length(line3) - static_cast<TYPE>(1)) < static_cast<TYPE>(0.001), L"AssertOrthogonal(): !line3 = %g." );
		assert_debug( fabs( dot(line1, line2) ) < static_cast<TYPE>(0.001f), L"AssertOrthogonal(): line1 * line2 = %g." );
		assert_debug( fabs( dot(line2, line3) ) < static_cast<TYPE>(0.001f), L"AssertOrthogonal(): line2 * line3 = %g." );
		assert_debug( fabs( dot(line1, line3) ) < static_cast<TYPE>(0.001f), L"AssertOrthogonal(): line1 * line3 = %g." );
	}
#else
	void AssertOrthogonal() const {} ;
#endif
};

template <typename TYPE>
class Matrix4x4 //все это, возможно, очень устарело - см. Matrix3x3
{
public:
	union 
	{
		struct 
		{
			TYPE        _11, _12, _13, _14;
			TYPE        _21, _22, _23, _24;
			TYPE        _31, _32, _33, _34;
			TYPE        _41, _42, _43, _44;
		};

		TYPE m[4][4];

		struct
		{
			Vector3D<TYPE> line1; TYPE w1;
			Vector3D<TYPE> line2; TYPE w2;
			Vector3D<TYPE> line3; TYPE w3;
			Vector3D<TYPE> line4; TYPE w4;
		};
	};

	Matrix4x4() {}
	Matrix4x4( const Vector3D<TYPE> &line1, 
		const Vector3D<TYPE> &line2, 
		const Vector3D<TYPE> &line3, 
		const Vector3D<TYPE> &line4 )
	{
		this->line1 = line1;	w1 = 0.f;
		this->line2 = line2;	w2 = 0.f;
		this->line3 = line3;	w3 = 0.f;
		this->line4 = line4;	w4 = 1.f;
	}

	Matrix4x4( const Vector3D<TYPE> &line1, TYPE w1,
		const Vector3D<TYPE> &line2, TYPE w2, 
		const Vector3D<TYPE> &line3, TYPE w3, 
		const Vector3D<TYPE> &line4, TYPE w4)
	{
		this->line1 = line1;	this->w1 = w1;
		this->line2 = line2;	this->w2 = w2;
		this->line3 = line3;	this->w3 = w3;
		this->line4 = line4;	this->w4 = w4;
	}

	Matrix4x4(const Matrix3x3<TYPE>& m, const Vector3D<float>& v)
	{
		this->line1 = m.line1; w1 = 0;
		this->line2 = m.line2; w2 = 0;
		this->line3 = m.line3; w3 = 0;
		this->line4 = v; w4 = 1.f;
	}

	Matrix4x4<TYPE>( const Matrix4x4<TYPE> &_m )
	{ 
		memcpy(m,_m.m,4*4*sizeof(TYPE)); 
	}

	Matrix4x4<TYPE>& operator = ( const Matrix4x4<TYPE>& _m ) 
	{ 
		memcpy(m,_m.m,4*4*sizeof(TYPE)); return *this; 
	}


	TYPE* operator [](int id) {return m[id];}
	const TYPE* operator [](int id) const {return m[id];}

	void zero()
	{
		memset(m,0,4*4*sizeof(TYPE));
	}

	void one()
	{
		zero();
		_11=1.f;	_22=1.f;	_33=1.f;	_44=1.f;
	}

	void SetDiagonal( Vector3D<TYPE>& in_vDiagonal )
	{
		_11 = in_vDiagonal.x;	_22 = in_vDiagonal.y;	_33 = in_vDiagonal.z;
	}

	inline Matrix4x4<TYPE> &operator *=( const TYPE f );

	inline void SetPosition( const Vector3D<TYPE>& in_vPosition );

	inline void RotateX( float angle );
	inline void RotateY( float angle );
	inline void RotateZ( float angle );

	inline void Transpose( const Matrix4x4 &in_Transform );

	void Transpose()
	{
		swap(_12, _21);
		swap(_13, _31);
		swap(_14, _41);
		swap(_23, _32);
		swap(_24, _42);
		swap(_34, _43);
	}

	//Строки матрицы (базис!)
	const Vector3D<TYPE> &x() const { return line1; }
	const Vector3D<TYPE> &y() const { return line2; }
	const Vector3D<TYPE> &z() const { return line3; }
	const Vector3D<TYPE> &o() const { return line4; }

	const Vector3D<TYPE> &forward() const { return line1; }
	const Vector3D<TYPE> &left() const { return line2; }
	const Vector3D<TYPE> &up() const { return line3; }

	//КРИВОЙ Базис матрицы - столбцы
	Vector3D<TYPE> kx() const { return Vector3D<TYPE>(m[0][0],m[1][0],m[2][0]); }
	Vector3D<TYPE> ky() const { return Vector3D<TYPE>(m[0][1],m[1][1],m[2][1]); }
	Vector3D<TYPE> kz() const { return Vector3D<TYPE>(m[0][2],m[1][2],m[2][2]); }

	void SetBasis( const Vector3D<TYPE> &forward,  //установка матрицы по базису
		const Vector3D<TYPE> &left, 
		const Vector3D<TYPE> &up )
	{
		line1 = forward; w1 = TYPE(0);
		line2 = left;	 w2 = TYPE(0);
		line3 = up;		 w3 = TYPE(0);
		line4.zero();	 w4 = TYPE(1);
	}

	void SetBasis(const Matrix3x3<TYPE>& m)
	{
		line1 = m.line1; w1 = TYPE(0);
		line2 = m.line2; w2 = TYPE(0);
		line3 = m.line3; w3 = TYPE(0);
		line4.zero();	 w4 = TYPE(1);
	}

	Vector3D<TYPE> GetAngles() const
	{
		Vector3D<TYPE> NewAA;
		if( TAbs<TYPE>( m[0][2])>1.f - 1e-6f )
		{
			NewAA.y = -m[0][2]*PI_BY_2;
			NewAA.x = 0.f;
			TYPE fFloat0 = m[1][1];
			CLIP_NUMBER(fFloat0, -1.f, 1.f);
			NewAA.z=-sign(m[1][0])*acosf(fFloat0);
		}
		else
		{
			TYPE fFloat0 = m[0][2];
			CLIP_NUMBER(fFloat0, -1.f, 1.f);
			NewAA.y=-asinf(fFloat0);
			int i=sign(m[2][2]), k=sign(m[0][0]);
			if (i==0) i=1; if (k==0) k=1;
			TYPE Float1=m[1][2]/TYPE(cosf(NewAA.y));
			if (Float1>1.f) Float1=1.f; if (Float1<-1.f) Float1=-1.f;
			NewAA.x=PI*TYPE(i==-1)+i*asinf(Float1);
			TYPE Float2=m[0][1]/TYPE(cosf(NewAA.y));
			if (Float2>1.f) Float2=1.f; if (Float2<-1.f) Float2=-1.f;
			NewAA.z=PI*TYPE(k==-1)+k*TYPE(asinf(Float2));
		}
		return NewAA;
	}

	static Matrix4x4<TYPE> Identity()
	{
		Matrix4x4<TYPE> m;
		m._11 = TYPE(1); m._12 = TYPE(0); m._13 = TYPE(0); m._14 = TYPE(0);
		m._21 = TYPE(0); m._22 = TYPE(1); m._23 = TYPE(0); m._24 = TYPE(0);
		m._31 = TYPE(0); m._32 = TYPE(0); m._33 = TYPE(1); m._34 = TYPE(0);
		m._41 = TYPE(0); m._42 = TYPE(0); m._43 = TYPE(0); m._34 = TYPE(1);
		return m;
	}


	void InverseTransform(const Matrix4x4<TYPE>& in);
	const Matrix4x4<TYPE>& InverseTransform();
};

//@{ Simplified declarations
typedef Matrix2x2<float> Matrix2x2f;
typedef Matrix3x3<float> Matrix3x3f;
typedef Matrix4x4<float> Matrix4x4f;

typedef Matrix2x2<double> Matrix2x2d;
typedef Matrix3x3<double> Matrix3x3d;
typedef Matrix4x4<double> Matrix4x4d;
//@}

#include "Matrix.inl"
