#include "TerrainObject.h"
#include "wgs84.h"

#include "Log.h"

#include <d3dx10math.h>


CTerrainObject::CTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
	const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, HeightfieldConverter* in_pHF)
	: _ID(ID), _params(in_pBlockDesc), _textureFileName(in_wsTextureFileName), _heightmapFileName(in_wsHeightmapFileName), _pHeightfieldConverter(in_pHF)
{
	_vPos = vm::Vector3df(in_coordsInfo.vPosition);
	_vXAxis = vm::Vector3df(in_coordsInfo.vXAxis);
	_vYAxis = vm::Vector3df(in_coordsInfo.vYAxis);
	_vZAxis = vm::Vector3df(in_coordsInfo.vZAxis);

	_vHalfsizes = (vm::Vector3df(in_coordsInfo.vBoundBoxMaximum) - vm::Vector3df(in_coordsInfo.vBoundBoxMinimum))*0.5;

	if (in_coordsInfo.vBoundBoxMaximum[1] < _vHalfsizes[1])
		_vPos -= _vYAxis * (_vHalfsizes[1] - in_coordsInfo.vBoundBoxMaximum[1]);
	else
		if (fabs(in_coordsInfo.vBoundBoxMinimum[1]) < _vHalfsizes[1])
			_vPos += _vYAxis * (_vHalfsizes[1] - fabs(in_coordsInfo.vBoundBoxMinimum[1]));

	//	_vAABBMin = vm::Vector3df(in_coordsInfo.vBoundBoxMinimum);
	//	_vAABBMax = vm::Vector3df(in_coordsInfo.vBoundBoxMaximum);
}

CTerrainObject::~CTerrainObject()
{
	if (_apQuadVertices)
		delete[] _apQuadVertices;
}


void CTerrainObject::GetBoundBoxCorners(D3DXVECTOR3 out_pvCorners[8]) const
{
	vm::Vector3df vPoints[8];

	vPoints[0] = _vPos + _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];
	vPoints[1] = _vPos + _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	vPoints[2] = _vPos + _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	vPoints[3] = _vPos + _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];

	vPoints[4] = _vPos - _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];
	vPoints[5] = _vPos - _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	vPoints[6] = _vPos - _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	vPoints[7] = _vPos - _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];

	for (int i = 0; i < 8; i++)
	{
		out_pvCorners[i] = D3DXVECTOR3(vPoints[i][0], vPoints[i][1], vPoints[i][2]);
	}
}

void CTerrainObject::CalculateReferencePoints(std::vector<vm::Vector3df>** out_pvecPoints, std::vector<vm::Vector3df>** out_pvecNormals) const
{
	if (_bReferencePointsCalulated)
	{
		*out_pvecPoints = &_vecRefPoints;
		*out_pvecNormals = &_vecRefNormals;

		return;
	}

	_vecRefPoints.resize(0);
	_vecRefNormals.resize(0);

	double dfMinLat = _params.fMinLattitude;
	double dfMaxLat = _params.fMaxLattitude;
	double dfMinLong = _params.fMinLongitude;
	double dfMaxLong = _params.fMaxLongitude;

	const int N = 5;

	double dfDeltaLat = (dfMaxLat - dfMinLat) / N;
	double dfDeltaLong = (dfMaxLong - dfMinLong) / N;

	double dfLat = dfMinLat;

	for (int i = 0; i < N + 1; i++)
	{
		double dfLong = dfMinLong;

		for (int j = 0; j < N + 1; j++)
		{
			vm::Vector3df vPoint = GetWGS84SurfacePoint(dfLong, dfLat);
			vm::Vector3df vNormal = GetWGS84SurfaceNormal(dfLong, dfLat);

			_vecRefPoints.push_back(vPoint);
			_vecRefNormals.push_back(vNormal);

			dfLong += dfDeltaLong;
		}

		dfLat += dfDeltaLat;
	}

	*out_pvecPoints = &_vecRefPoints;
	*out_pvecNormals = &_vecRefNormals;

	_bReferencePointsCalulated = true;
}

void CTerrainObject::initVertexBuffer()
{
	if (!_pTriangulation)
	{
		LogMessage("Error: CTerrainObject::initVertexBuffer() - triangulation is null");
		return;
	}

	if (!_pTriangulation->pVertexBuffer || !_pTriangulation->pIndexBuffer)
	{
		LogMessage("Error: CTerrainObject::initVertexBuffer() - triangulation vertex or index buffers are null");
		return;
	}

	_apQuadVertices = new SVertex [(_pTriangulation->nCountLat - 1)*(_pTriangulation->nCountLong - 1) * 6];
		
	_pHeightfieldConverter->UnmapTriangulation(_pTriangulation, _apQuadVertices, nullptr);
}

void CTerrainObject::calculatePreciseBoundBox()
{
}

// returns true if in_vPos is above
bool CTerrainObject::CalculateProjectionOnSurface(const vm::Vector3df& in_vPos, vm::Vector3df& out_vProjection, vm::Vector3df& out_vNormal)
{
	double dfLong, dfLat;
	bool isPositionAboveBlock = GetObjectManager()->GetClippedProjection(_ID, in_vPos, dfLat, dfLong);

	if (!_apQuadVertices)
	{
		out_vProjection = GetWGS84SurfacePoint(dfLong, dfLat) * _pHeightfieldConverter->GetWorldScale();
		out_vNormal = GetWGS84SurfaceNormal(dfLong, dfLat);
		return isPositionAboveBlock;
	}

	STerrainBlockParams params;
	GetObjectManager()->ComputeTerrainObjectParams(_ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfLatCoeff = (dfLat - dfMinLat) / (dfMaxLat - dfMinLat);
	double dfLongCoeff = (dfLong - dfMinLong) / (dfMaxLong - dfMinLong);

	unsigned int iQuadLat = static_cast<unsigned int>(round((_pTriangulation->nCountLat - 2) * dfLatCoeff));
	unsigned int iQuadLong = static_cast<unsigned int>(round((_pTriangulation->nCountLong - 2) * dfLongCoeff));

	double dfQuadLatSize = (dfMaxLat - dfMinLat) / (_pTriangulation->nCountLat - 1);
	double dfQuadLongSize = (dfMaxLong - dfMinLong) / (_pTriangulation->nCountLong - 1);

	double dfQuadMinLat = dfMinLat + iQuadLat * dfQuadLatSize;
	double dfQuadMaxLat = dfMinLat + iQuadLat * dfQuadLatSize + dfQuadLatSize;
	double dfQuadMinLong = dfMinLong + iQuadLong * dfQuadLongSize;
	double dfQuadMaxLong = dfMinLong + iQuadLong * dfQuadLongSize + dfQuadLongSize;

	double dfQuadLatCoeff = (dfLat - dfQuadMinLat) / (dfQuadMaxLat - dfQuadMinLat);
	double dfQuadLongCoeff = (dfLong - dfQuadMinLong) / (dfQuadMaxLong - dfQuadMinLong);

	struct QuadVertex
	{
		vm::Vector3df pos, normal;

		QuadVertex() {}
		QuadVertex(const SVertex& v) : pos(v.position.x, v.position.y, v.position.z), normal(v.normal.x, v.normal.y, v.normal.z) {}
		QuadVertex(const vm::Vector3df& p, const vm::Vector3df& n) : pos(p), normal(n) {}

		static QuadVertex lerp(double k, const QuadVertex& v0, const QuadVertex& v1)
		{
			return QuadVertex(
				v0.pos + k * (v1.pos - v0.pos),
				vm::normalize(v0.normal + k * (v1.normal - v0.normal))
			);
		}
	};

	QuadVertex quadVertices[4];

	SVertex* pQuadStartVertex = &_apQuadVertices[(iQuadLong * (_pTriangulation->nCountLat - 1) + iQuadLat) * 6];

	quadVertices[0] = pQuadStartVertex[0];
	quadVertices[1] = pQuadStartVertex[1];
	quadVertices[2] = pQuadStartVertex[2];
	quadVertices[3] = pQuadStartVertex[5];

	QuadVertex p0 = QuadVertex::lerp(dfQuadLatCoeff, quadVertices[1], quadVertices[0]);
	QuadVertex p1 = QuadVertex::lerp(dfQuadLatCoeff, quadVertices[2], quadVertices[3]);

	QuadVertex result = QuadVertex::lerp(dfQuadLongCoeff, p0, p1);

	out_vProjection = result.pos;
	out_vNormal = result.normal;

	return isPositionAboveBlock;
}
