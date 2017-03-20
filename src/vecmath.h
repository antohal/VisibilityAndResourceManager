#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <string.h>
#include <math.h>

#include <vector>
#include <list>

#include "Debug.h"

using namespace std;

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define FRAND static_cast<float>(rand())/static_cast<float>(RAND_MAX)
#define RND_DELTA_PART(a, d) a + d*2*(FRAND - 0.5f)
#define RND_A_B(a, b) a + FRAND*(b - a)

#ifndef D2R
#define D2R M_PI/180.0
#endif


#ifndef R2D
#define R2D 180.0/M_PI
#endif

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define ARRAY_END(array) ((array) + ARRAY_SIZE(array))

#ifndef ZERO                                  
    #define ZERO    1.e-48          
#endif

namespace vm 
{

//@{ Useful functions
template <typename T>
inline void CLIP_NUMBER(T& tNum, const T& A, const T& B)
{
    if(tNum < A) tNum = A;
    if(tNum > B) tNum = B;
}

template <typename T, typename Real>
inline T lerp (const Real& alpha, const T& A, const T& B)
{
	return A + alpha*(B - A);
}

template <typename T>
inline bool IsEqualT(const T A, const T B, const T tEpsilon = T(0.0001))
{
    return static_cast<T>(fabs(static_cast<double>(A - B))) < tEpsilon;
}

template <typename T>
inline int sign(const T A)
{
    if(A > 0)
        return 1;
    else if(A < 0)
        return -1;

    return 0;
}

template <typename T>
inline T Sign(const T A)
{
    if (A > 0)
        return 1;
    return -1;
}

template <typename T>
inline bool GetBit(T A, size_t bit)
{
    T B = A >> bit;
    return B & T(1);
}

template <typename T>
inline T TAbs (const T& A)
{
	if (A < 0) return -A;
	return A;
}

template <typename T>
inline T round (const T& A)
{
	return static_cast<T>(floor(A + 0.5));
}

//@}

//basic vector template
template <typename Type, int N>
class Vector
{
public:
    Type	v[N];

    //@{ Constructors
    Vector () {}

    explicit Vector (const Type in_tInit)
    {
        for(size_t i = 0; i < N; ++i)
            v[i] = in_tInit;
    }

    //usability constructors
    Vector (const std::vector<Type>& in_vecValues)
    {
		assert_debug(N == in_vecValues.size(), L"Wrong vector dimension!");
        for(size_t i = 0; i < N; ++i)
            v[i] = in_vecValues[i];
    }

	Vector(const std::initializer_list<Type>& in_list) : Vector(std::vector<Type>(in_list))
	{
	}

    Vector (const Type& x, const Type& y)
    {
		static_assert(N == 2, "Wrong vector dimension!");
        v[0] = x;
        v[1] = y;
    }

    Vector (const Type& x, const Type& y, const Type& z)
    {
		static_assert(N == 3, "Wrong vector dimension!");
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }

    Vector (const Type& x, const Type& y, const Type& z, const Type& w)
    {
		static_assert(N == 4, "Wrong vector dimension!");
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = w;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5)
    {
		static_assert(N == 5, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6)
    {
		static_assert(N == 6, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7)
    {
		static_assert(N == 7, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7, const Type& x8)
    {
		static_assert(N == 8, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7, const Type& x8, const Type& x9)
    {
		static_assert(N == 9, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
        v[8] = x9;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7, const Type& x8,
            const Type& x9, const Type& x10)
    {
        static_assert(N == 10, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
        v[8] = x9;
        v[9] = x10;
    }

    Vector (const Type x1, const Type x2, const Type x3, const Type x4, const Type x5, const Type x6, const Type x7, const Type x8,
            const Type x9, const Type x10, const Type x11)
    {
		static_assert(N == 11, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
        v[8] = x9;
        v[9] = x10;
        v[10] = x11;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7, const Type& x8,
            const Type& x9, const Type& x10, const Type& x11, const Type& x12)
    {
		static_assert(N == 12, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
        v[8] = x9;
        v[9] = x10;
        v[10] = x11;
        v[11] = x12;
    }

    Vector (const Type& x1, const Type& x2, const Type& x3, const Type& x4, const Type& x5, const Type& x6, const Type& x7, const Type& x8,
            const Type& x9, const Type& x10, const Type& x11, const Type& x12, const Type& x13)
    {
		static_assert(N == 13, "Wrong vector dimension!");
        v[0] = x1;
        v[1] = x2;
        v[2] = x3;
        v[3] = x4;
        v[4] = x5;
        v[5] = x6;
        v[6] = x7;
        v[7] = x8;
        v[8] = x9;
        v[9] = x10;
        v[10] = x11;
        v[11] = x12;
        v[12] = x13;
    }

    //This allows to convert from vector of other type (if static conversion exist)
    template <typename Type2>
    Vector (const Vector<Type2, N>& in_vVec)
    {
        for(size_t i = 0; i < N; ++i)
            v[i] = static_cast<Type>(in_vVec.v[i]);
    }
    //@}

    //@{ Misc functions
    void	Zero ()
    {
        memset(&v[0], 0, N*sizeof(Type));
    }

	inline int Dim () const {return N;}

    inline Type& operator [] (size_t in_nIndex)
    {
		assert_debug(in_nIndex < N, L"Access out of collection bounds");
        return v[in_nIndex];
    }

    inline const Type& operator [] (size_t in_nIndex) const
    {
		assert_debug(in_nIndex < N, L"Access out of collection bounds");
        return v[in_nIndex];
    }

    inline void ToSTLVector (vector<Type>& out_vec) const
    {
        out_vec.resize(N);
        for(size_t i = 0; i < N; ++i)
            out_vec[i] = v[i];
    }
    //@}

    //@{ Self operators
    inline const Vector<Type, N>& operator += (const Vector<Type, N>& in_vVec)
    {
        for(size_t i = 0; i < N; ++i)
            v[i] += in_vVec.v[i];
        return *this;
    }

    inline const Vector<Type, N>& operator -= (const Vector<Type, N>& in_vVec)
    {
        for(size_t i = 0; i < N; ++i)
            v[i] -= in_vVec.v[i];
        return *this;
    }

	template<typename Type2>
	inline const Vector<Type, N>& operator *= (const Type2& in_tScalar)
    {
        for(size_t i = 0; i < N; ++i)
            v[i] = static_cast<Type>(v[i]*in_tScalar);
        return *this;
    }

	void ClipValues (const Type minValue, const Type maxValue)
	{
        for(size_t i = 0; i < N; ++i)
			CLIP_NUMBER(v[i], minValue, maxValue);
	}
    //@}
};

//@{ type definitions
typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 4> Vector4f;

typedef Vector<double, 2> Vector2df;
typedef Vector<double, 3> Vector3df;
typedef Vector<double, 4> Vector4df;


typedef Vector<int, 2> Vector2i;
typedef Vector<int, 3> Vector3i;
typedef Vector<int, 4> Vector4i;


//@}

//@{ basic vector functions

template <typename Type, int N>
inline Vector<Type, N> Sin (const Vector<Type, N>& in_vA)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = static_cast<Type>(sin(static_cast<double>(in_vA.v[i])));
	return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> Cos(const Vector<Type, N>& in_vA)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = static_cast<Type>(cos(static_cast<double>(in_vA.v[i])));
	return vResult;
}


template <typename Type, int N>
inline Vector<Type, N> operator - (const Vector<Type, N>& in_vA)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = -in_vA.v[i];
    return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator + (const Vector<Type, N>& in_vA, const Vector<Type, N>& in_vB)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = in_vA.v[i] + in_vB.v[i];
    return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator * (const Vector<Type, N>& in_vA, const Vector<Type, N>& in_vB)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = in_vA.v[i] * in_vB.v[i];
	return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator + (const Vector<Type, N>& in_vA, const Type& in_fB)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = in_vA.v[i] + in_fB;
	return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator + (const Type& in_fB, const Vector<Type, N>& in_vA)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = in_vA.v[i] + in_fB;
	return vResult;
}


template <typename Type, int N>
inline Vector<Type, N> operator - (const Vector<Type, N>& in_vA, const Type& in_fB)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = in_vA.v[i] - in_fB;
	return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator - (const Type& in_fB, const Vector<Type, N>& in_vA)
{
	Vector<Type, N> vResult;
	for (size_t i = 0; i < N; ++i)
		vResult[i] = in_fB - in_vA.v[i];
	return vResult;
}

template <typename Type, int N>
inline Vector<Type, N> operator - (const Vector<Type, N>& in_vA, const Vector<Type, N>& in_vB)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = in_vA.v[i] - in_vB.v[i];
    return vResult;
}

template <typename Type, typename Type2, int N>
inline Vector<Type, N> operator * (const Type2& in_tScalar, const Vector<Type, N>& in_vB)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = static_cast<Type>(in_tScalar*in_vB.v[i]);
    return vResult;
}

template <typename Type, typename Type2, int N>
inline Vector<Type, N> operator * (const Vector<Type, N>& in_vA, const Type2& in_tScalar)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = static_cast<Type>(in_tScalar*in_vA.v[i]);
    return vResult;
}


template <typename Type, typename Type2, int N>
inline Vector<Type, N> operator / (const Vector<Type, N>& in_vA, const Type2& in_tScalar)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
        vResult[i] = static_cast<Type>(in_vA.v[i]/in_tScalar);
    return vResult;
}

template <typename Type, int N>
inline Type dot(const Vector<Type, N>& in_vA, const Vector<Type, N>& in_vB)
{
    Type tResult(0);
    for(size_t i = 0; i < N; ++i)
        tResult += in_vA.v[i]*in_vB.v[i];
    return tResult;
}

template <typename Type, int N>
inline Type length(const Vector<Type, N>& in_vVec)
{
    return static_cast<Type>(sqrt(dot(in_vVec, in_vVec)));
}

template <typename Type, int N>
inline Vector<Type, N> normalize(const Vector<Type, N>& in_vVec)
{
    const Type tInvLength = Type(1)/length(in_vVec);
    Vector<Type, N> vRes;
    for(size_t i = 0; i < N; ++i)
        vRes[i] = in_vVec.v[i]*tInvLength;
    return vRes;
}

template <typename Type>
inline Vector<Type, 3> cross(const Vector<Type, 3>& in_vA, const Vector<Type, 3>& in_vB)
{
    return Vector<Type, 3>(	in_vA[1]*in_vB[2] - in_vA[2]*in_vB[1],
                            -in_vA[0]*in_vB[2] + in_vA[2]*in_vB[0],
                            in_vA[0]*in_vB[1] - in_vA[1]*in_vB[0]);
}

template <typename Type>
inline Vector<Type, 3> CrossProduct(const Vector<Type, 3>& in_vA, const Vector<Type, 3>& in_vB)
{
	return cross(in_vA, in_vB);
}

template <typename T, int N>
inline bool IsEqualVector(const Vector<T, N>& A, const Vector<T, N>& B, const T tEpsilon = T(0.0001))
{
    bool bEqual = true;
    for(size_t i = 0; i < N; ++i)
        if(!IsEqualT(A[i], B[i], tEpsilon))
        {
            bEqual = false;
            break;
        }
    return bEqual;
}
//@}


//basic matrix template
template <typename Type, int N, int M>
class Matrix
{
public:
    Vector<Type, M>		m[N];

    //@{ Constructors
    Matrix() {}

    //Usability constructors
    explicit Matrix (Type in_Val)
    {
        for(size_t i = 0; i < N; ++i)
            for(size_t j = 0; j < M; ++j)
                m[i][j] = in_Val;
    }

    Matrix (const vector<Type>& in_vecValues)
    {
		assert_debug(M*N == in_vecValues.size(), L"Wrong matrix dimension!");
        for(size_t i = 0; i < N; ++i)
            for(size_t j = 0; j < M; ++j)
                m[i][j] = in_vecValues[i*N + j];
    }

	Matrix (const vector<Vector<Type, M>>& in_vecRows)
	{
		assert_debug(N == in_vecRows.size(), L"Wrong matrix dimensions!");

		for (size_t i = 0; i < N; i++)
		{
			m[i] = in_vecRows[i];
		}
	}

	Matrix(const std::initializer_list<Vector<Type, M>>& in_rows) : Matrix(vector<Vector<Type, M>>(in_rows))
	{
	}

    Matrix(const Vector<Type, M>& v1, const Vector<Type, M>& v2)
    {
		static_assert(N == 2, "Wrong matrix dimensions!");
        m[0] = v1;
        m[1] = v2;
    }

    Matrix(const Vector<Type, M>& v1, const Vector<Type, M>& v2, const Vector<Type, M>& v3)
    {
		static_assert(N == 3, "Wrong matrix dimensions!");
        m[0] = v1;
        m[1] = v2;
        m[2] = v3;
    }

    Matrix(const Vector<Type, M>& v1, const Vector<Type, M>& v2, const Vector<Type, M>& v3,
           const Vector<Type, M>& v4)
    {
		static_assert(N == 4, "Wrong matrix dimensions!");
        m[0] = v1;
        m[1] = v2;
        m[2] = v3;
        m[3] = v4;
    }
    //@}

	bool IsEqual (const Matrix<Type, N, M>& mInput, const Type& tol = Type(0.0001))
	{
		for(size_t i = 0; i < N; ++i)
			for(size_t j = 0; j < M; ++j)
				if (! IsEqualT(m[i][j], mInput[i][j], tol))
					return false;

		return true;
	}

	inline const Matrix<Type, N, M>& operator += (const Matrix<Type, N, M>& mInput)
	{
		for(size_t i = 0; i < N; ++i)
			for(size_t j = 0; j < M; ++j)
				m[i][j] += mInput[i][j];

		return *this;
	}

    //{@ Misc functions
	inline const Type& at (size_t i, size_t j) const
	{
		assert_debug(i < N, L"Out of bounds");
		return m[i][j];
	}

	inline Type& at (size_t i, size_t j)
	{
		assert_debug(i < N, L"Out of bounds");
		return m[i][j];
	}

	inline Vector<Type, M>& operator [](size_t i)
    {
		assert_debug(i < N, L"Out of bounds");
        return m[i];
    }

    inline const Vector<Type, M>& operator [](size_t i) const
    {
		assert_debug(i < N, L"Out of bounds");
        return m[i];
    }

    inline void Zero()
    {
        for(size_t i = 0; i < N; ++i)
            m[i].Zero();
    }

    inline bool DiagM(Vector<Type, N>& out_vDiag) const
    {
		assert_debug(M >= N, L"wrong output vector chosen");
        if(M < N)
            return false;

        for(size_t i = 0; i < N; ++i)
            out_vDiag[i] = m[i][i];

        return true;
    }

    inline bool DiagN(Vector<Type, M>& out_vDiag) const
    {
		assert_debug(N >= M, L"wrong output vector chosen");
        if(N < M)
            return false;

        for(size_t i = 0; i < M; ++i)
            out_vDiag[i] = m[i][i];

        return true;
    }

    inline void Fill(const Type in_val)
    {
        for(size_t i = 0; i < M; ++i)
            for(size_t j = 0; j < N; ++j)
                m[i][j] = in_val;
    }

    inline static const Matrix<Type, N, M>& Identity()
    {
        static Matrix<Type, N, M> mIdentity;
        static bool bCalculated = false;
        if(!bCalculated)
        {
            mIdentity.Zero();
            for(size_t i = 0, iMax = std::min<size_t>(N, M); i < iMax; ++i)
                mIdentity[i][i] = 1;
            bCalculated = true;
        }
        return mIdentity;
    }

	inline static const Matrix<Type, N, M>& ZeroMatrix()
    {
        static Matrix<Type, N, M> mZero;
        static bool bCalculated = false;
        if(!bCalculated)
        {
            mZero.Zero();
            bCalculated = true;
        }
        return mZero;
    }

    inline void	SetRow(const Vector<Type, M>& in_vRow, size_t in_iRowID)
    {
		assert_debug(in_iRowID < N, L"wrong row idx!");
        m[in_iRowID] = in_vRow;
    }

    inline void	SetColumn(const Vector<Type, N>& in_vCol, size_t in_iColID)
    {
		assert_debug(in_iColID < M, L"wrong column idx!");
        for(size_t i = 0; i < N; ++i)
            m[i][in_iColID] = in_vCol[i];
    }

    inline Vector<Type, N>	GetColumn(size_t in_iColID)
    {
		assert_debug(in_iColID < M, L"wrong column idx!");
        Vector<Type, N> vCol;
        for(size_t i = 0; i < N; ++i)
            vCol[i] = m[i][in_iColID];
        return vCol;
    }

    //}
};

//@{ Typedefs
typedef Matrix<float, 2, 2> Matrix2x2f;
typedef Matrix<double, 2, 2> Matrix2x2df;

typedef Matrix<float, 3, 3> Matrix3x3f;
typedef Matrix<double, 3, 3> Matrix3x3df;

typedef Matrix<float, 4, 4> Matrix4x4f;
typedef Matrix<double, 4, 4> Matrix4x4df;

typedef Matrix<float, 4, 3> Matrix4x3f;
typedef Matrix<double, 4, 3> Matrix4x3df;


template <typename Real>
class InertiaTensor : public Matrix<Real, 3, 3>
{
public:

	InertiaTensor () {}

	InertiaTensor (const Matrix<Real, 3, 3>& in_m)
		: Matrix<Real, 3, 3>(in_m)
	{
		AssertSymmetric();
	}

    InertiaTensor (const Vector<Real, 3>& v1, const Vector<Real, 3>& v2, const Vector<Real, 3>& v3)
		: Matrix<Real, 3, 3>(v1, v2, v3)
	{
		AssertSymmetric();
	}

	void ToMassCenter (Real in_Mass, const Vector<Real, 3>& in_vMassCenter)
	{
		Real cx = in_vMassCenter[0];
		Real cy = in_vMassCenter[1];
		Real cz = in_vMassCenter[2];

		Real rx2 = (in_vMassCenter[1]*in_vMassCenter[1] + in_vMassCenter[2]*in_vMassCenter[2]);
		Real ry2 = (in_vMassCenter[0]*in_vMassCenter[0] + in_vMassCenter[2]*in_vMassCenter[2]);
		Real rz2 = (in_vMassCenter[1]*in_vMassCenter[1] + in_vMassCenter[0]*in_vMassCenter[0]);

		Real Ixx = Matrix<Real, 3, 3>::m[0][0] - in_Mass*rx2;
		Real Iyy = Matrix<Real, 3, 3>::m[1][1] - in_Mass*ry2;
		Real Izz = Matrix<Real, 3, 3>::m[2][2] - in_Mass*rz2;

		Real Ixy = Matrix<Real, 3, 3>::m[0][1] - in_Mass*cx*cy;
		Real Izx = Matrix<Real, 3, 3>::m[2][0] - in_Mass*cz*cx;
		Real Iyz = Matrix<Real, 3, 3>::m[1][2] - in_Mass*cy*cz;

		Matrix<Real, 3, 3>::m[0][0] = Ixx; Matrix<Real, 3, 3>::m[0][1] = Ixy; Matrix<Real, 3, 3>::m[0][2] = Izx;
		Matrix<Real, 3, 3>::m[1][0] = Ixy; Matrix<Real, 3, 3>::m[1][1] = Iyy; Matrix<Real, 3, 3>::m[1][2] = Iyz;
		Matrix<Real, 3, 3>::m[2][0] = Izx; Matrix<Real, 3, 3>::m[2][1] = Iyz; Matrix<Real, 3, 3>::m[2][2] = Izz;

		AssertSymmetric();
	}

	bool AssertSymmetric ()
	{
		bool isSymmetric = Matrix<Real, 3, 3>::m[0][1] == Matrix<Real, 3, 3>::m[1][0]
            && Matrix<Real, 3, 3>::m[0][2] == Matrix<Real, 3, 3>::m[2][0]
            && Matrix<Real, 3, 3>::m[1][2] == Matrix<Real, 3, 3>::m[2][1];

		assert_debug(isSymmetric,	L"Inertia tensor must be symmetric!");

		if (! isSymmetric)
			wcout << L"Inertia tensor must be symmetric" << endl;

		return isSymmetric;
	}
};

typedef InertiaTensor<double>	InertiaTensorD;
typedef InertiaTensor<float>	InertiaTensorF;

//@}

//@{ Matrix functions
template <typename Type, int M, int N, int K>
inline Matrix<Type, M, K> mul(const Matrix<Type, M, N>& mA, const Matrix<Type, N, K>& mB)
{
    Matrix<Type, M, K> mResult;
    for(size_t i = 0; i < M; ++i)
    {
        for(size_t j = 0; j < K; ++j)
        {
            Type fDotResult = 0;
            for(size_t k = 0; k < N; ++k)
                fDotResult += mA[i][k]*mB[k][j];
            mResult[i][j] = fDotResult;
        }
    }
    return mResult;
}

template <typename Type, int M, int N>
inline Vector<Type, M> mul(const Matrix<Type, M, N>& mA, const Vector<Type, N>& vB)
{
    Vector<Type, M> vResult;
    for(size_t i = 0; i < M; ++i)
    {
        vResult[i] = dot(mA[i], vB);
    }
    return vResult;
}

template <typename Type, int M, int N>
inline Vector<Type, M> mul_vec(const Matrix<Type, M, N>& mA, const Matrix<Type, N, 1>& vB)
{
    Vector<Type, M> vResult;
    Matrix<Type, M, 1> mResult = mul(mA, vB);

    for(size_t i = 0; i < M; ++i)
        vResult[i] = mResult[i][0];

    return vResult;
}

template <typename Type, int M, int N>
inline Vector<Type, N> mul(const Vector<Type, M>& vA, const Matrix<Type, M, N>& mB)
{
    Vector<Type, N> vResult;
    for(size_t i = 0; i < N; ++i)
    {
        Type tDot = 0;
        for(size_t j = 0; j < M; j++)
            tDot += mB[i][j]*vA[j];
        vResult[i] = tDot;
    }
    return vResult;
}


template <typename Type, int M, int N>
inline Matrix<Type, N, M> transposed(const Matrix<Type, M, N>& mA)
{
    Matrix<Type, N, M> mResult;
    for(size_t i = 0; i < M; ++i)
        for(size_t j = 0; j < N; ++j)
            mResult[j][i] = mA[i][j];
    return mResult;
}

template <typename Type>
inline Matrix<Type, 3, 3> axis_rotation(const Type tAngle, const Vector<Type, 3>& vAxis)
{
    Type tCos = static_cast<Type>(cos(static_cast<double>(tAngle)));
    Type tSin = static_cast<Type>(sin(static_cast<double>(tAngle)));
    Vector<Type, 3> vNormAxis = normalize(vAxis);
    Type x = vNormAxis[0], y = vNormAxis[1], z = vNormAxis[2];

    return Matrix<Type, 3, 3> (	Vector<Type, 3>(tCos + (1 - tCos)*x*x,		(1 - tCos)*x*y - tSin*z,	(1 - tCos)*x*z + tSin*y),
                                Vector<Type, 3>((1 - tCos)*y*x + tSin*z,	tCos + (1 - tCos)*y*y,		(1 - tCos)*y*z - tSin*x),
                                Vector<Type, 3>((1 - tCos)*z*x - tSin*y,	(1 - tCos)*z*y + tSin*x,	tCos + (1 - tCos)*z*z) );
}

template <typename Type>
inline Matrix<Type, 3, 3> rotation_x(const Type tAngle)
{
    Type tCos = static_cast<Type>(cos(static_cast<double>(tAngle)));
    Type tSin = static_cast<Type>(sin(static_cast<double>(tAngle)));

    return Matrix<Type, 3, 3>(	Vector<Type, 3>(1,	0,	0),
                                Vector<Type, 3>(0,	tCos,	tSin),
                                Vector<Type, 3>(0, 	-tSin,	tCos));
}

template <typename Type>
inline Matrix<Type, 3, 3> rotation_y(const Type tAngle)
{
    Type tCos = static_cast<Type>(cos(static_cast<double>(tAngle)));
    Type tSin = static_cast<Type>(sin(static_cast<double>(tAngle)));

    return Matrix<Type, 3, 3>(	Vector<Type, 3>(tCos,	0,	-tSin),
                                Vector<Type, 3>(0,	1,	0),
                                Vector<Type, 3>(tSin, 	0,	tCos));
}

template <typename Type>
inline Matrix<Type, 3, 3> rotation_z(const Type tAngle)
{
    Type tCos = static_cast<Type>(cos(static_cast<double>(tAngle)));
    Type tSin = static_cast<Type>(sin(static_cast<double>(tAngle)));

    return Matrix<Type, 3, 3>(	Vector<Type, 3>(tCos,	tSin,	0),
                                Vector<Type, 3>(-tSin,	tCos,	0),
                                Vector<Type, 3>(0, 	0,	1));
}

template <typename Type>
inline Matrix<Type, 3, 3> euler_213(const Type y, const Type x, const Type z)
{
	Matrix<Type, 3, 3> matrix;

	matrix[0][0] = cos(y) * cos(z) + sin(y) * sin(x) * sin(z);
	matrix[0][1] = cos(x) * sin(z);
	matrix[0][2] = cos(y) * sin(x) * sin(z) - cos(z) * sin(y);
	matrix[1][0] = cos(z) * sin(y) * sin(x) - cos(y) * sin(z);
	matrix[1][1] = cos(x) * cos(z);
	matrix[1][2] = sin(y) * sin(z) + cos(y) * cos(z) * sin(x);
	matrix[2][0] = cos(x) * sin(y);
	matrix[2][1] = -1 * sin(x);
	matrix[2][2] = cos(y) * cos(x);

	return matrix;
}

template <typename Type>
inline Matrix<Type, 3, 3> euler_angles(const Type tAlpha, const Type tBetta, const Type tGamma)
{
    Matrix<Type, 3, 3> mRot1 = rotation_z(tAlpha);
    Matrix<Type, 3, 3> mRot2 = rotation_x(tBetta);
    Matrix<Type, 3, 3> mRot3 = rotation_z(tGamma);

    return mul(mul(mRot1, mRot2), mRot3);
}

template <typename Type>
inline Matrix<Type, 3, 3> plane_angles(const Type tAlpha, const Type tBetta, const Type tGamma)
{
    Matrix<Type, 3, 3> mRot1 = rotation_y(tAlpha);
    Matrix<Type, 3, 3> mRot2 = rotation_x(tBetta);
    Matrix<Type, 3, 3> mRot3 = rotation_z(tGamma);

    return mul(mul(mRot1, mRot2), mRot3);
}
//@}

//@{ Quaternion
template <typename Type>
class Quaternion
{
public:
    Type q[4];

    Quaternion()
    {
        q[0] = 1;
        q[1] = 0;
        q[2] = 0;
        q[3] = 0;
    }

    Quaternion(const Type* in_pData)
    {
        q[0] = in_pData[0];
        q[1] = in_pData[1];
        q[2] = in_pData[2];
        q[3] = in_pData[3];
    }

    Quaternion(const Vector<Type, 4>& in_vVec)
    {
        q[0] = in_vVec[0];
        q[1] = in_vVec[1];
        q[2] = in_vVec[2];
        q[3] = in_vVec[3];
    }

	void Set (Type q0, Type q1, Type q2, Type q3)
	{
        q[0] = q0;
        q[1] = q1;
        q[2] = q2;
        q[3] = q3;
	}

    Quaternion(Type q0, Type q1, Type q2, Type q3)
    {
		Set(q0, q1, q2, q3);
    }

    // из оси и угла
    Quaternion(Type tAngle, const Vector<Type, 3>& vAxis)
    {
       MakeFromAxisAngle(tAngle, vAxis);
    }

    // из углов эйлера
    Quaternion(Type a1, Type a2, Type a3)
    {
        q[0] = -cos(a1)*cos(a2)*cos(a3)+sin(a1)*sin(a2)*sin(a3);
        q[1] = cos(a1)*sin(a2)*sin(a3)+sin(a1)*cos(a2)*cos(a3);
        q[2] = cos(a1)*cos(a2)*sin(a3)+sin(a1)*sin(a2)*cos(a3);
        q[3] = cos(a1)*sin(a2)*cos(a3)-sin(a1)*cos(a2)*sin(a3);
    }

	// кратчайший поворот, перевод€щий ось 1 в ось 2
	Quaternion(const Vector<Type, 3>& axis1, const Vector<Type, 3>& axis2)
	{
		Vector<Type, 3> normAxis1 = normalize(axis1);
		Vector<Type, 3> normAxis2 = normalize(axis2);

		if (IsEqualVector<Type, 3>(normAxis1, normAxis2))
		{
			q[0] = 1; q[1] = q[2] = q[3] = 0;
		}
		else
		{
			Vector<Type, 3> crossProd = cross(normAxis1, normAxis2);
			Type lengthCross = length(crossProd);
			Vector3df rotationAxis = crossProd/lengthCross;
			Type angle = static_cast<Type>(asin(lengthCross));

			if (dot(normAxis1, normAxis2) < 0)
				angle = M_PI - angle;

			MakeFromAxisAngle(angle, rotationAxis);
		}
	}

    // получение кватерниона из матрицы направл€ющих косинусов
    Quaternion(const Matrix<Type, 3, 3>& in_m)
    {
        Type tr, s;

		Matrix<Type, 3, 3> m(transposed(in_m));

        tr = m[0][0] + m[1][1] + m[2][2] + 1;

        if(tr > Type(0))
        {
            s = 0.5f/Type(sqrt(tr));
            q[3] = 0.25/s;

            q[0] = (m[2][1] - m[1][2])*s;
            q[1] = (m[0][2] - m[2][0])*s;
            q[2] = (m[1][0] - m[0][1])*s;
        }
        else
        {
            // наибольший элемент на главной диагонали
            //столбец 0:
            if(m[0][0] >= m[1][1] && m[0][0] >= m[2][2])
            {
                s = Type(sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]))*2.f;

                q[0] = 0.5/s;
                q[1] = (m[0][1] + m[1][0])/s;
                q[2] = (m[0][2] + m[2][0])/s;
                q[3] = (m[1][2] + m[2][1])/s;
            }
            else if(m[1][1] >= m[0][0] && m[1][1] >= m[2][2])
            {
                s = Type(sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]))*2.f;

                q[0] = (m[0][1] + m[1][0])/s;
                q[1] =  0.5/s;
                q[2] = (m[1][2] + m[2][1])/s;
                q[3] = (m[0][2] + m[2][0])/s;
            }
            else if(m[2][2] >= m[0][0] && m[2][2] >= m[1][1])
            {
                s = Type(sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]))*2.f;

                q[0] = (m[0][2] + m[2][0])/s;
                q[1] = (m[1][2] + m[2][1])/s;
                q[2] = 0.5/s;
                q[3] = (m[0][1] + m[1][0])/s;
            }
        }

		Quaternion<Type> qres(q[3], q[0], q[1], q[2]);
		q[0] = qres[0];
		q[1] = qres[1];
		q[2] = qres[2];
		q[3] = qres[3];
    }


	void MakeFromAxisAngle(Type tAngle, const Vector<Type, 3>& vAxis)
    {
        q[0] = static_cast<Type>(cos(static_cast<double>(tAngle)/2.0));
        Vector<Type, 3> vAlpha = vAxis*static_cast<Type>(sin(static_cast<double>(tAngle)/2.0));
        q[1] = vAlpha[0];
        q[2] = vAlpha[1];
        q[3] = vAlpha[2];
    }

    // в углы эйлера
    void GetEulerAngles (Type& roll, Type& yaw, Type& pitch)
    {
		Type x = q[1], y = q[2], z = q[3];
		Type w = q[0];
		Type test = x*y + z*w;

		if (test > 0.499)
		{ 
			// singularity at north pole

			pitch = 2 * atan2(x, w);
			yaw = M_PI*0.5;
			roll = 0;

			return;
		}
		if (test < -0.499) 
		{ 
			// singularity at south pole

			pitch = -2 * atan2(x, w);
			yaw = - M_PI*0.5;
			roll = 0;

			return;
		}

		Type sqx = x*x;
		Type sqy = y*y;
		Type sqz = z*z;

		pitch = atan2(2*y*w-2*x*z , 1 - 2*sqy - 2*sqz);
		yaw = asin(2*test);
		roll = atan2(2*x*w-2*y*z , 1 - 2*sqx - 2*sqz);
    }

    // в углы  рылова
    void GetKrylovAngles (Type& psi, Type& phi, Type& theta)
    {
        Matrix<Type, 3, 3> m = ToSO3Operator();

        // kurs angle
        psi = atan2(m[0][1], m[0][0]);

        // tangazh
        theta = -asin(m[0][2]);

        // kren
        phi = atan2(m[1][2],m[2][2]);
    }

    const Type& operator [] (size_t idx) const
    {
		assert_debug(idx < 4, L"Out of quaternion bounds");
        return q[idx];
    }

    Type& operator [] (size_t idx)
    {
		assert_debug(idx < 4, L"Out of quaternion bounds");
        return q[idx];
    }

    // нормирование
    void Normalize ()
    {
        Type tLen = length(Vector<Type, 4>(q[0], q[1], q[2], q[3]));
        if(IsEqualT<Type>(tLen, Type(0.0001)))
            return;

        Type tInvLen = Type(1)/tLen;
        q[0] *= tInvLen;
        q[1] *= tInvLen;
        q[2] *= tInvLen;
        q[3] *= tInvLen;
    }

	void Rand ()
	{
		q[0] = 2*FRAND - 1;
		q[1] = 2*FRAND - 1;
		q[2] = 2*FRAND - 1;
		q[3] = 2*FRAND - 1;
	}

    // преобразование в матрицу направл€ющих косинусов
    Matrix<Type, 3, 3>	ToSO3Operator() const
    {
		return Matrix<Type, 3, 3>(
		Vector<Type,3>( q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3], 2 * (q[1] * q[2] + q[0] * q[3]), 2 * (q[1] * q[3] - q[0] * q[2]) ),
		Vector<Type, 3>(2*(q[1]*q[2] - q[0]*q[3]), q[0] * q[0] - q[1] * q[1] + q[2] * q[2] - q[3] * q[3], 2 * (q[2] * q[3] + q[0] * q[1])),
		Vector<Type, 3>(2*(q[1] * q[3] + q[0] * q[2]), 2 * (q[2] * q[3] - q[0] * q[1]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3] )
		);
    }

    // —опр€женный кватернион
    Quaternion<Type>	GetConjugate() const
    {
        return Quaternion<Type>(q[0], -q[1], -q[2], -q[3]);
    }

    // норма кватерниона
    Type		Norm() const
    {
        return q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    }

    // обратный кватернион
    Quaternion<Type>	GetInverse() const
    {
        return GetConjugate()*(1.f/Norm());
    }
};

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniondf;

template <typename Type>
inline Quaternion<Type> operator *(const Quaternion<Type>& q1, const Quaternion<Type>& q2)
{
    Type w1 = q1.q[0], w2 = q2.q[0];
    Vector<Type, 3> v1(q1.q[1], q1.q[2], q1.q[3]);
    Vector<Type, 3> v2(q2.q[1], q2.q[2], q2.q[3]);

    Vector<Type, 3> vRes = cross(v1, v2) + w1*v2 + w2*v1;

    return Quaternion<Type>(w1*w2 - dot(v1, v2), vRes[0], vRes[1], vRes[2]);
}

template <typename Type>
inline Quaternion<Type> operator *(const Quaternion<Type>& q1, const Type tScalar)
{
    return Quaternion<Type>(q1.q[0]*tScalar, q1.q[1]*tScalar, q1.q[2]*tScalar, q1.q[3]*tScalar);
}

template <typename Type>
inline Quaternion<Type> operator *(const Type tScalar, const Quaternion<Type>& q1)
{
    return Quaternion<Type>(q1.q[0]*tScalar, q1.q[1]*tScalar, q1.q[2]*tScalar, q1.q[3]*tScalar);
}

template <typename Type>
inline Quaternion<Type> operator -(const Quaternion<Type>& q1, const Quaternion<Type>& q2)
{
    return Quaternion<Type>(q1.q[0] - q2.q[0], q1.q[1] - q2.q[1], q1.q[2] - q2.q[2], q1.q[3] - q2.q[3]);
}

template <typename Type>
inline Quaternion<Type> operator +(const Quaternion<Type>& q1, const Quaternion<Type>& q2)
{
    return Quaternion<Type>(q1.q[0] + q2.q[0], q1.q[1] + q2.q[1], q1.q[2] + q2.q[2], q1.q[3] + q2.q[3]);
}
//@}



//*********************************************************************//
//
//      Gaussian Solution of System of Linear Equations.
//
//*************************** 24.04.2007 ******************************//

class SLU_Gauss
{
public:
    SLU_Gauss ( unsigned int k, const double * const * a );
    bool solve ( const double * b, double * x ) const; // b[n], x[n]
    bool solve ( const double * const * a, const double * b, double * x ) const;
    double condition () const
    {
        return cond;
    }
    double determinant () const;
    ~SLU_Gauss ();
    void destroy();

private:
    const unsigned int n;
    unsigned int * ipvt;
    double ** m_pA;
    double cond;
// noncopyable
    SLU_Gauss ( SLU_Gauss & );
    void operator = ( SLU_Gauss & );

};

// In this include such functions are described:
#include "mathmisc.h"

/*
template<typename Real, int nRow, int nCol>
		inline bool SVD(const Matrix<Real, nRow, nCol>& in_A, Matrix<Real, nRow, nRow>& U,
				Matrix<Real, nRow, nCol>& W, Matrix<Real, nCol, nCol>& V);


template <typename Real, int M>
 		 inline bool inverse(const Matrix<Real, M, M>& in_A, Matrix<Real, M, M>& out_B);


template <typename Real, int M, int N>
		 Matrix<Real, N, M>	pseudo_inverse(const Matrix<Real, M, N>& in_A)

*/

class DynamicMatrix
{
public:

	DynamicMatrix (size_t M, size_t N) : _M(M), _N(N), _numThreads(2)
	{
		_mat = new double [M*N];
	}

	~ DynamicMatrix ()
	{
		delete [] _mat;
	}

	inline const double& at (size_t i, size_t j) const
	{
		assert_debug(i < _M, L"illegal dynamic matrix index");
		assert_debug(j < _N, L"illegal dynamic matrix index");
		return _mat[i*_N + j];
	}

	inline double& at (size_t i, size_t j)
	{
		assert_debug(i < _M, L"illegal dynamic matrix index");
		assert_debug(j < _N, L"illegal dynamic matrix index");
		return _mat[i*_N + j];
	}

	void multiply (const double* in_vec, double* out_vec);

private:

	DynamicMatrix (const DynamicMatrix&) : _mat(NULL), _M(0), _N(0), _numThreads(2)
	{
	}

	struct ThreadStruct
	{
		DynamicMatrix* _mat;
		const double* _inVec;
		double* _outVec;
		size_t _i1, _i2;
	};

	static void* ProcessThread (void* pData);

	double*	_mat;
	size_t	_M, _N;

	const size_t _numThreads;

};


template <typename T>
class Delayer
{
public:

	Delayer () 
		: _dfHistoryUpdateInterval(1.0)
		, _dfLastHistoryUpdateTime(0.0)
	{
	}

	Delayer (const double& in_dfHistoryUpdateInterval)
		: _dfHistoryUpdateInterval(in_dfHistoryUpdateInterval)
		, _dfLastHistoryUpdateTime(0.0)
	{
	}

	// for external control with parameters
	double& HistoryUpdateIntervalRef ()
	{
		return _dfHistoryUpdateInterval;
	}

	void AddToHistory (const T& in_tVal, const double& in_dfCurrentTime, const double& in_dfMaxDelay)
	{
		if (in_dfCurrentTime - _dfLastHistoryUpdateTime < _dfHistoryUpdateInterval)
			return;

		_lstHistory.push_front(make_pair(in_tVal, in_dfCurrentTime));

		while (! _lstHistory.empty() && in_dfCurrentTime - _lstHistory.back().second >= in_dfMaxDelay)
		{
			_lstHistory.pop_back();
		}

		_dfLastHistoryUpdateTime = in_dfCurrentTime;
	}

	bool GetDelayedValue (const double& in_dfCurrentTime, const double& in_dfDelay, T& out_val) const
	{
		if (_lstHistory.empty())
			return false;

		if (in_dfDelay <= 0)
			return false;

		double dfTime = in_dfCurrentTime - in_dfDelay;
#ifndef _MSC_VER
#ifndef __APPLE__
		for (struct std::list<std::pair<T,double> >::const_iterator it = _lstHistory.begin(); it != _lstHistory.end(); it ++)
		{
			struct std::list<std::pair<T,double> >::const_iterator it_next = it; it_next++;
#else
        for (typename std::list<std::pair<T,double> >::const_iterator it = _lstHistory.begin(); it != _lstHistory.end(); it ++)
        {
                typename std::list<std::pair<T,double> >::const_iterator it_next = it; it_next++;
            
#endif
#else
		for (std::list<std::pair<T,double> >::const_iterator it = _lstHistory.begin(); it != _lstHistory.end(); it ++)
		{
			std::list<std::pair<T,double> >::const_iterator it_next = it; it_next++;
#endif
			if (it_next == _lstHistory.end())
			{
				out_val = it->first;
				return true;
			}
		
			double dfThisTime = it->second;
			double dfPastTime = it_next->second;

			if (dfTime >= dfPastTime && dfTime < dfThisTime)
			{
				out_val = lerp((dfTime - dfPastTime)/(dfThisTime - dfPastTime), it->first, it_next->first);
				return true;
			}
		}

		out_val = _lstHistory.back().first;
		return true;
	}

private:

	std::list<std::pair<T, double> >	_lstHistory;
	double								_dfHistoryUpdateInterval;
	double								_dfLastHistoryUpdateTime;
};

}; // namespace vm
