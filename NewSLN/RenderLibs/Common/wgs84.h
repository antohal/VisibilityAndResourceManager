#pragma once

#include "vecmath.h"

vm::Vector3df GetWGS84SurfacePoint(double longitude, double lattitude);
vm::Vector3df GetWGS84SurfaceNormal(const vm::Vector3df& in_vSurfacePoint);
vm::Vector3df GetWGS84SurfaceNormal(double longitude, double lattitude);
void GetWGS84LongLatHeight(const vm::Vector3df& vPoint, double& out_long, double& out_lat, double& out_height, double& out_len);
double GetWGS84Height(const vm::Vector3df& vPoint);
double GetWGS84Radius(const vm::Vector3df& vPoint);

bool IsSegmentIntersectsEarthMinRadius(const vm::Vector3df& from, const vm::Vector3df& to);
