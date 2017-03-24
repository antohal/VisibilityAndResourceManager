#pragma once

#include "Geometry/Vector3D.h"
#include "Geometry/Plane.h"
#include <vector>

class CollectObjectsData
{
public:

	CollectObjectsData() {}
	CollectObjectsData(const Vector3D<float>& pos, const Vector3D<float>& dir, const Vector3D<float>& up,
		float nearPlane, float farPlane, float horizontalFov, float verticalFov);

	const CBoundBox<float>& GetBoundBox() const { return _boundBox; }
	const CFrustum<float>&	GetFrustum() const { return _frustum; }
	const Vector3D<float>&	GetPos() const { return _pos; }

private:

	CBoundBox<float>	_boundBox;
	CFrustum<float>		_frustum;
	Vector3D<float>		_pos;
};

class IVisibilityManagerPrivateInterface
{
public:

	virtual void	MarkPotentiallyVisibleObjects(const std::vector<CollectObjectsData>& in_vecCloud) = 0;

};
