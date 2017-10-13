#pragma once

#include "Face.h"
#include "BoundBox.h"
#include <vector>

struct ConvexHull
{
	std::vector<Vector3f>	vertices;
	std::vector<Face>		faces;
	CBoundBox<float>		boundBox;
};

bool BuildConvexHull(const std::vector<Vector3f>& in_vecVertices, ConvexHull& out_convexHull);
