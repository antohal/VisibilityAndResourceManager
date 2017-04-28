#pragma once

#include "vecmath.h"

vm::Vector3df GetWGS84SurfacePoint(double longitude, double lattitude);
vm::Vector3df GetWGS84SurfaceNormal(const vm::Vector3df& in_vSurfacePoint);
vm::Vector3df GetWGS84SurfaceNormal(double longitude, double lattitude);
