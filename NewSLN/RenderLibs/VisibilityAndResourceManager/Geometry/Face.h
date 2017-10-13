#pragma once

#include "Vector3D.h"

class Face
{
	Vector3f	normal = Vector3f(0);
	size_t		indices[3];
};
