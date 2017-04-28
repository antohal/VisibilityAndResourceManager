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

	return vm::Vector3df(
		R*cosA*cosB,
		R*sinA*cosB,
		R*sinB
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
