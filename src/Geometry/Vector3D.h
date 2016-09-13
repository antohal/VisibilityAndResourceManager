#pragma once

template<typename TYPE>
class	Vector3D;

template<typename TYPE>
class	Vector2D
{
public:
	union
	{
		struct
		{
			TYPE	x, y;
		};
		TYPE v[2];
	};

	Vector2D() {}
	explicit Vector2D ( const TYPE& v ) { y = x = v; }
	Vector2D ( const Vector2D<TYPE>& v ) { x = v.x; y = v.y; }
	explicit Vector2D ( const Vector3D<TYPE>& v ) { x = v.x; y = v.y; }
	Vector2D ( const TYPE& vx, const TYPE& vy ) { x = vx; y = vy; }

	template<typename TYPE2>
	explicit Vector2D ( const Vector2D<TYPE2>& v ) { x = static_cast<TYPE>(v.x); y = static_cast<TYPE>(v.y); }

	inline Vector2D<TYPE> operator += ( const Vector2D<TYPE>& v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	inline Vector2D<TYPE> operator -= ( const Vector2D<TYPE>& v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	inline Vector2D<TYPE> operator *= ( const TYPE& f )
	{
		x *= f;
		y *= f;
		return *this;
	}

	inline Vector2D<TYPE> operator /= ( const TYPE& f )
	{
		TYPE ff = 1 / f;
		x *= ff;
		y *= ff;
		return *this;
	}

	TYPE	LengthSq() const
	{
		return static_cast<TYPE>(x*x + y*y);
	}

	TYPE	Length() const
	{
		return static_cast<TYPE>( sqrt( LengthSq() ) );
	}

	inline Vector2D<TYPE> Normalize()
	{
		TYPE tInv = TYPE(1)/Length();
		x*=tInv;
		y*=tInv;
		return *this;
	}

	inline TYPE NormalizeL()
	{
		TYPE tLen = Length();
		TYPE tInv = TYPE(1)/tLen;
		x*=tInv;
		y*=tInv;
		return tLen;
	}

	inline void zero()
	{
		x = TYPE(0);
		y = TYPE(0);
	}

	inline Vector2D<TYPE> &RotateVector( TYPE angle )
	{ 
		float cosA, sinA;
		SinCos(angle, sinA, cosA);
		float _x = x * cosA - y * sinA;
		y = x * sinA + y * cosA;
		x = _x;
		return *this; 
	}

	Vector2D<TYPE>& operator = ( const Vector3D<TYPE>& v ) { 
		x = v.x; y = v.y; return *this; 
	}

	bool operator == ( const Vector2D<TYPE> &v ) const
	{
		return (x==v.x) && (y==v.y);
	}

	bool operator !=(const Vector2D<TYPE> &v) const {
		return (x!=v.x) || (y!=v.y);
	}
};

template<typename TYPE>
class	Vector3D
{
public:
	union
	{
		struct
		{
			TYPE	x, y, z;
		};
		TYPE v[3];
	};

	Vector3D() {}
	explicit Vector3D ( const TYPE& v ) { z = y = x = v; }

	template<typename TYPE2>
	explicit Vector3D ( const Vector3D<TYPE2>& v ) { x = static_cast<TYPE>(v.x); y = static_cast<TYPE>(v.y); z = static_cast<TYPE>(v.z); }

	Vector3D ( const Vector2D<TYPE>& v, const TYPE& a ) { x = v.x; y = v.y; z = a; }
	Vector3D ( const TYPE& vx, const TYPE& vy, const TYPE& vz ) { x = vx; y = vy; z = vz; }

	void RandomGaussSlope( TYPE tAmplitude );
	TYPE	GetMaxValue() const
	{
		return std::max<float>(x, std::max<float>(y, z));
	}

	inline Vector3D<TYPE> operator += ( const Vector3D<TYPE>& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector3D<TYPE> operator -= ( const Vector3D<TYPE>& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector3D<TYPE> operator *= ( const TYPE& f )
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	inline Vector3D<TYPE> operator /= ( const TYPE& f )
	{
		TYPE ff = 1 / f;
		x *= ff;
		y *= ff;
		z *= ff;
		return *this;
	}

	inline void UpdateMin(const Vector3D<TYPE>& u)
	{
		if (x>u.x) x=u.x; 
		if (y>u.y) y=u.y; 
		if (z>u.z) z=u.z;
	}

	inline void UpdateMax(const Vector3D<TYPE>& u)
	{
		if (x<u.x) x=u.x; 
		if (y<u.y) y=u.y; 
		if (z<u.z) z=u.z;
	}

	TYPE	LengthSq() const
	{
		return static_cast<TYPE>( x*x + y*y + z*z );
	}

	TYPE	Length() const
	{
		return static_cast<TYPE>( sqrt( LengthSq() ) );
	}

	inline Vector3D<TYPE>& Normalize()
	{
		TYPE tInv = TYPE(1)/Length();
		x*=tInv;
		y*=tInv;
		z*=tInv;
		return *this;
	}

	inline TYPE NormalizeL()
	{
		TYPE tLen = Length();
		TYPE tInv = TYPE(1)/tLen;
		x*=tInv;
		y*=tInv;
		z*=tInv;
		return tLen;
	}

	inline void zero()
	{
		x = TYPE(0);
		y = TYPE(0);
		z = TYPE(0);
	}

	bool operator ==(const Vector3D<TYPE> &v) const {
		return (x==v.x)&&(y==v.y)&&(z==v.z);
	}

	bool operator !=(const Vector3D<TYPE> &v) const {
		return (x!=v.x)||(y!=v.y)||(z!=v.z);
	}

};

template <typename Type>
class	Vector4D
{
public:

	union
	{
		struct
		{
			Type	x, y, z, w;
		};
		Type v[4];
	};

	Vector4D() {}

	explicit Vector4D( const Type& in_a )
	{
		x = y = z = w = in_a;
	}

	Vector4D(const Vector3D<Type>& in_vPoint, const Type& in_w )
	{
		x	=	in_vPoint.x;
		y	=	in_vPoint.y;
		z	=	in_vPoint.z;
		w	=	in_w;
	}
	Vector4D( const Type& in_x, const Type& in_y, const Type& in_z, const Type& in_w )
	{
		x	=	in_x;
		y	=	in_y;
		z	=	in_z;
		w	=	in_w;
	}
};

// --- Vector2D operators ---
template<typename TYPE>
inline Vector2D<TYPE> operator *( const Vector2D<TYPE>& v, const TYPE& a )
{
	return Vector2D<TYPE>( v.x*a, v.y*a );
}

template<typename TYPE>
inline Vector2D<TYPE> operator *( const TYPE& a, const Vector2D<TYPE>& v )
{
	return Vector2D<TYPE>( v.x*a, v.y*a );
}

template<typename TYPE>
inline Vector2D<TYPE> operator -( const Vector2D<TYPE>& v1, const Vector2D<TYPE>& v2 )
{
	return Vector2D<TYPE>( v1.x - v2.x, v1.y - v2.y );
}

template<typename TYPE>
inline Vector2D<TYPE> operator -( const Vector2D<TYPE>& v1 )
{
	return Vector2D<TYPE>( -v1.x, -v1.y );
}

template<typename TYPE>
inline Vector2D<TYPE> operator +( const Vector2D<TYPE>& v1, const Vector2D<TYPE>& v2 )
{
	return Vector2D<TYPE>( v1.x + v2.x, v1.y + v2.y );
}

template<typename TYPE>
inline Vector2D<TYPE> Normalize( const Vector2D<TYPE>& v )
{
	TYPE tInv = TYPE(1)/v.Length();
	return v*tInv;
}

template<typename TYPE>
inline Vector2D<TYPE> Perpendicular( const Vector2D<TYPE>& v )
{
	return Vector2D<TYPE>( -v.y, v.x );
}

template<typename TYPE>
inline TYPE Length( const Vector2D<TYPE>& v )
{
	return v.Length();
}

template<typename TYPE>
inline TYPE SqrLen( const Vector2D<TYPE> &v )
{ 
	return v.x*v.x + v.y*v.y; 
}


//////////////////////////////////////////////////////
template<typename TYPE>
inline Vector3D<TYPE> operator /( const Vector3D<TYPE>& v, const TYPE& a )
{
	TYPE inv = static_cast<TYPE>(1) / a;
	return Vector3D<TYPE>( v.x*inv, v.y*inv, v.z*inv );
}

template<typename TYPE>
inline Vector3D<TYPE> operator *( const Vector3D<TYPE>& v, const TYPE& a )
{
	return Vector3D<TYPE>( v.x*a, v.y*a, v.z*a );
}

template<typename TYPE>
inline Vector3D<TYPE> operator *( const TYPE& a, const Vector3D<TYPE>& v )
{
	return Vector3D<TYPE>( v.x*a, v.y*a, v.z*a );
}

template<typename TYPE>
inline Vector3D<TYPE> operator -( const Vector3D<TYPE>& v1, const Vector3D<TYPE>& v2 )
{
	return Vector3D<TYPE>( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

template<typename TYPE>
inline Vector3D<TYPE> operator +( const Vector3D<TYPE>& v1, const Vector3D<TYPE>& v2 )
{
	return Vector3D<TYPE>( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

template<typename TYPE>
inline Vector3D<TYPE> operator -( const Vector3D<TYPE>& v1 )
{
	return Vector3D<TYPE>( -v1.x, -v1.y, -v1.z );
}

template<typename TYPE>
inline TYPE Length( const Vector3D<TYPE>& v )
{
	return v.Length();
}

template<typename TYPE>
inline TYPE LengthSq( const Vector3D<TYPE>& v )
{
	return v.LengthSq();
}

template<typename TYPE>
inline Vector3D<TYPE> Normalize( const Vector3D<TYPE>& v )
{
	TYPE tInv = TYPE(1)/v.Length();
	return v*tInv;
}

template<typename T> 
inline Vector3D<T> scale(const Vector3D<T>& a, const Vector3D<T>& b)
{
	return Vector3D<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template<typename T> 
inline Vector2D<T> scale(const Vector2D<T>& a, const Vector2D<T>& b)
{
	return Vector2D<T>(a.x * b.x, a.y * b.y);
}

template<typename TYPE>
inline TYPE SqrLen( const Vector3D<TYPE> &v )
{ 
	return v.x*v.x + v.y*v.y + v.z*v.z; 
}

template <typename TYPE>
inline TYPE DotProduct( const Vector3D<TYPE> &v1,  const Vector3D<TYPE> &v2 )
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
template <typename TYPE>
inline TYPE DotProduct( const Vector2D<TYPE> &v1,  const Vector2D<TYPE> &v2 )
{
	return v1.x*v2.x + v1.y*v2.y;
}

#define dot DotProduct

template <typename TYPE>
inline Vector3D<TYPE> CrossProduct( const Vector3D<TYPE> &u,  const Vector3D<TYPE> &v )
{
	return Vector3D<TYPE>( u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x );
}
#define cross CrossProduct

template<typename TYPE>
inline Vector3D<TYPE> Perpendicular( const Vector3D<TYPE>& u )
{
	Vector3D<TYPE> cr;
	if( (TAbs<TYPE>(u.x) <= TAbs<TYPE>(u.y)) && (TAbs<TYPE>(u.x) <= TAbs<TYPE>(u.z))) 
		cr = Vector3D<TYPE>(TYPE(1),TYPE(0),TYPE(0));
	else
		if((TAbs<TYPE>(u.y) <= TAbs<TYPE>(u.x)) && (TAbs<TYPE>(u.y) <= TAbs<TYPE>(u.z))) 
			cr = Vector3D<TYPE>(0,1,0);
		else
			cr = Vector3D<TYPE>(0,0,1);

	return Normalize( cross( u, cr ) );
}

template<>
inline void Vector3D<float>::RandomGaussSlope(float Amplitude)
{
	float L = ::Length(*this);
	if (L <= 0.001f) return;

	float delta = float(L * tan(Amplitude));
	float dx,dy,dz, R2;
	//выбор случайного вектора внутри сферы
	do
	{
		dx = -1 + 2 * float(rand())/RAND_MAX;
		dy = -1 + 2 * float(rand())/RAND_MAX;
		dz = -1 + 2 * float(rand())/RAND_MAX;
		R2 = ((dx*dx) + (dy*dy) + (dz*dz));
	}
	while (R2 > 1);

	float fDistance = float(rand())/RAND_MAX;
	float fFactor = delta * fDistance; //в центре - гуще

	x += fFactor * dx; y += fFactor * dy; z += fFactor * dz;
	float fNewLength = ::Length(*this);
	float Mult = L / fNewLength;
	x *= Mult; y *= Mult; z *= Mult;
}

template<typename TYPE>
inline Vector4D<TYPE> operator *( const Vector4D<TYPE>& v, const TYPE& a )
{
	return Vector4D<TYPE>( v.x*a, v.y*a, v.z*a, v.w*a );
}

//@{ simplified declarations
typedef Vector2D<float> Vector2f;
typedef Vector3D<float> Vector3f;
typedef Vector4D<float> Vector4f;

typedef Vector2D<double> Vector2d;
typedef Vector3D<double> Vector3d;
typedef Vector4D<double> Vector4d;
//@}

template <typename Type>
inline bool IsEqualVector3D (const Vector3D<Type>& a, const Vector3D<Type>& b, Type in_fEpsilon = Type(FLT_MIN))
{
	return IsEqualT(a.x, b.x, in_fEpsilon) && IsEqualT(a.y, b.y, in_fEpsilon) && IsEqualT(a.z, b.z, in_fEpsilon);
}
