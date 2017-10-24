#include "wgs84.h"

vm::Vector3df GetWGS84SurfacePoint(double longitude, double lattitude)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	double cosB = cos(lattitude);
	double sinB = sin(lattitude);

	double cosA = cos(longitude);
	double sinA = sin(longitude);

	double R = sqrt(Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB));

	/*return vm::Vector3df(
		R*cosA*cosB,
		R*sinA*cosB,
		R*sinB
	);*/

	return vm::Vector3df(
		R*sinA*cosB,
		R*sinB,
		-R*cosA*cosB
	);
}

vm::Vector3df GetWGS84SurfaceNormal(const vm::Vector3df& in_vSurfacePoint)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	vm::Vector3df vUnnormalizedNormal = vm::Vector3df(
		2 * in_vSurfacePoint[0] / (Rmax*Rmax),
		2 * in_vSurfacePoint[1] / (Rmax*Rmax),
		2 * in_vSurfacePoint[2] / (Rmin*Rmin)
	);

	return normalize(vUnnormalizedNormal);
}

vm::Vector3df GetWGS84SurfaceNormal(double longitude, double lattitude)
{
	return GetWGS84SurfaceNormal(GetWGS84SurfacePoint(longitude, lattitude));
}

void GetWGS84LongLatHeight(const vm::Vector3df& vPoint, double& out_long, double& out_lat, double& out_height, double& out_len)
{
	vm::Vector3df vDir = vm::normalize(vPoint);

	out_lat = asin(vDir[1]);
	out_long = atan2(vDir[0], -vDir[2]);

	if (out_long < 0)
		out_long = 2 * M_PI + out_long;

	vm::Vector3df vSurfacePoint = GetWGS84SurfacePoint(out_long, out_lat);

	out_len = vm::length(vPoint);
	out_height = vm::length(vPoint - vSurfacePoint);
}

bool IsSegmentIntersectsEarthMinRadius(const vm::Vector3df& A, const vm::Vector3df& B)
{
	// A lies outside sphere

	const double Rmin = 6356752.3142;

	vm::Vector3df C(0.0);
	vm::Vector3df O = A;
	vm::Vector3df D = vm::normalize(B - A);

	vm::Vector3df L = C - O;
	double tca = vm::dot_prod(L, D);

	if (tca < 0)
		return false;

	double d2 = vm::length2(L) - tca*tca;

	if (d2 < 0)
		return false;

	double d = sqrt(d2);

	// ray misses the sphere
	if (d > Rmin)
		return false;

	double thc = sqrt(Rmin*Rmin - d*d);

	double t0 = tca - thc;
	double t1 = tca + thc;


	if (vm::length(B - A) < t0)
		return false;

	return true;
}
