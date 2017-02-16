#pragma once

#include "Geometry/Vector3D.h"
#include "Geometry/BoundBox.h"
#include "Geometry/Plane.h"
#include "Geometry/Matrix.h"

namespace VisibilityManager {

class CCamera
{
public:

	CCamera ();

//	void CreateFromMatrix(const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, float in_fFarPlane, float in_fNearPlane, float in_fHorizontalFOV,
//		float in_fVerticalFOV, const Matrix4x4<float>& in_mView, const Matrix4x4<float> in_mProj);

	void CreateFromFrustumPoints(const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp,
		float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane, const Vector3f in_avViewSpaceFrustumPoints[8]);

	void SetPerspective (const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, 
		float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane);

	void SetOrtho (const Vector3f& in_vPos, const Vector3f& in_vDir, const Vector3f& in_vUp, const Vector3f& in_vSizes);

	const Vector3f&	GetPos () const;
	const Vector3f&	GetDir () const;
	const Vector3f& GetUp() const;
	const CFrustum<float>& GetFrustum () const;
	const CBoundBox<float>& GetBoundBox () const;

	bool	IsOrtho () const;
	float	GetNearPlane () const;
	float	GetFarPlane() const;

	float	GetHorizontalHalfFovTan () const;
	float	GetVerticalHalfFovTan () const;

	float GetHorizontalFov() const {
		return _fHorizontalFov;
	}

	float GetVerticalFov() const {
		return _fVerticalFov;
	}

private:

	Vector3f								_vPos;
	Vector3f								_vDir;
	Vector3f								_vUp;

	CFrustum<float>							_Frustum;
	CBoundBox<float>						_BBox;

	bool									_bOrtho;
	float									_fNearPlane;
	float									_fFarPlane;

	float									_fHorizontalHalfFovTan;
	float									_fVerticalHalfFovTan;

	float									_fHorizontalFov;
	float									_fVerticalFov;
};

};