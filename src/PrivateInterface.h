#pragma once

#include "Geometry/Vector3D.h"
#include <vector>

class IVisibilityManagerPrivateInterface
{
public:

	virtual void	SetPotentialVisibilityConvexCloud(const std::vector<Vector3f>& in_vecCloud) = 0;

};
