#include "stdafx.h"
#include "Camera.h"
#include "Geometry\InFrustum.h"

namespace VisibilityManager {

CCamera::CCamera () : _vPos(0), _bOrtho(false), _fNearPlane(1), _fFarPlane(100), _fHorizontalHalfFovTan(0), _fVerticalHalfFovTan(0)
{
}

//void CCamera::CreateFromMatrix(const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, float in_fFarPlane, float in_fNearPlane, float in_fHorizontalFOV,
//	float in_fVerticalFOV, const Matrix4x4<float>& in_mView, const Matrix4x4<float> in_mProj)
//{
//	_vDir = in_vDir;
//	_vPos = in_vPos;
//	_vUp = in_vUp;
//
//	SFOV_Tan HorizontalFOV(in_fHorizontalFOV);
//	SFOV_Tan VerticalFOV(in_fVerticalFOV);
//
//	Matrix3x3f mOrientation = GetMatrixFromForwardDirection<float>(Normalize(in_vDir), Normalize(in_vUp));
//
//	_fHorizontalHalfFovTan = HorizontalFOV.GetTanFOVAngle();
//	_fVerticalHalfFovTan = VerticalFOV.GetTanFOVAngle();
//
//	UpdateFrustumPlanes(mOrientation, GetPos(), in_fFarPlane, in_fNearPlane,
//		HorizontalFOV.GetTanFOVAngle(), VerticalFOV.GetTanFOVAngle(), mul(in_mView,in_mProj), _BBox, _Frustum);
//
//	_bOrtho = false;
//}


void CCamera::CreateFromFrustumPoints(const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp,
	float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane, const Vector3f in_avViewSpaceFrustumPoints[8])
{
	_vDir = in_vDir;
	_vPos = in_vPos;
	_vUp = in_vUp;

	_fNearPlane = in_fNearPlane;
	_fFarPlane = in_fFarPlane;

	Matrix3x3f mOrientation = GetMatrixFromForwardDirection<float>(Normalize(in_vDir), Normalize(in_vUp));

	SFOV_Tan HorizontalFOV(in_fHorizontalFOV);
	SFOV_Tan VerticalFOV(in_fVerticalFOV);

	_fHorizontalFov = in_fHorizontalFOV;
	_fVerticalFov = in_fVerticalFOV;

	_fHorizontalHalfFovTan = HorizontalFOV.GetTanFOVAngle();
	_fVerticalHalfFovTan = VerticalFOV.GetTanFOVAngle();

	UpdateFrustumPlanes(mOrientation, GetPos(), in_avViewSpaceFrustumPoints, in_fFarPlane, in_fNearPlane, _BBox, _Frustum);

	_bOrtho = false;
}

void CCamera::SetPerspective (const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, 
		float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane)
{
	_vDir = in_vDir;
	_vPos = in_vPos;
	_vUp = in_vUp;

	_fNearPlane = in_fNearPlane;
	_fFarPlane = in_fFarPlane;

	Matrix3x3f mOrientation = GetMatrixFromForwardDirection<float>(Normalize(in_vDir), Normalize(in_vUp));

	SFOV_Tan HorizontalFOV(in_fHorizontalFOV);
	SFOV_Tan VerticalFOV(in_fVerticalFOV);

	_fHorizontalHalfFovTan = HorizontalFOV.GetTanFOVAngle();
	_fVerticalHalfFovTan = VerticalFOV.GetTanFOVAngle();

	_fHorizontalFov = in_fHorizontalFOV;
	_fVerticalFov = in_fVerticalFOV;

	UpdateFrustumPlanes(mOrientation, GetPos(), in_fFarPlane, in_fNearPlane, 
		HorizontalFOV.GetTanFOVAngle(), VerticalFOV.GetTanFOVAngle(), 
			_BBox, _Frustum);

	_bOrtho = false;
}

float CCamera::GetHorizontalHalfFovTan () const
{
	return _fHorizontalHalfFovTan;
}

float CCamera::GetVerticalHalfFovTan () const
{
	return _fVerticalHalfFovTan;
}


void CCamera::SetOrtho (const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, const Vector3f& in_vSizes)
{
	_vPos = in_vPos;
	_vDir = in_vDir;
	Matrix3x3f mOrientation = GetMatrixFromForwardDirection<float>(Normalize(in_vDir), Normalize(in_vUp));

	UpdateFrustumPlanes(mOrientation, GetPos(), 0.5f*in_vSizes, _BBox, _Frustum);

	_bOrtho = true;
}

bool CCamera::IsOrtho () const
{
	return _bOrtho;
}

float	CCamera::GetNearPlane () const
{
	return _fNearPlane;
}

float	CCamera::GetFarPlane() const
{
	return _fFarPlane;
}

const Vector3f& CCamera::GetPos () const 
{
	return _vPos;
}

const Vector3f&	CCamera::GetDir () const
{
	return _vDir;
}

const Vector3f& CCamera::GetUp() const
{
	return _vUp;
}

const CFrustum<float>& CCamera::GetFrustum () const 
{
	return _Frustum;
}

const CBoundBox<float>& CCamera::GetBoundBox () const 
{
	return _BBox;
}

};
