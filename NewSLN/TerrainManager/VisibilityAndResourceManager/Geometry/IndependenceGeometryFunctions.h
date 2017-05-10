#pragma once

#ifndef PI
#define PI		(3.141592653589793f)
#endif //PI

#define PI_BY_2 (1.570796326795f)
#define PI_BY_4 (0.785398163397f)
#define TWO_PI	(6.283185307179586f)

#ifndef D2R
#define D2R 0.017453292519943295f //градусы в радианы
#endif

#ifndef R2D
#define R2D 57.29577951308232087f //радианы в градусы
#endif

#define DEG2RAD D2R
#define RAD2DEG R2D

template <typename T> inline void SinCos( const T& in_Angle, T& oSin, T& oCos );

template<>
inline void SinCos<float>( const float& in_fAngle, float& o_fSin, float& o_fCos )
{
	o_fSin = sinf( in_fAngle );
	o_fCos = cosf( in_fAngle );
}

template<>
inline void SinCos<double>( const double& in_dAngle, double& o_dSin, double& o_dCos )
{
	o_dSin = sin( in_dAngle );
	o_dCos = cos( in_dAngle );
}