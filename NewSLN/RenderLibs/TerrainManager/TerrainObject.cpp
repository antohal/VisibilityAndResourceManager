#include "TerrainObject.h"
#include "wgs84.h"

#include "AsyncTask.h"
#include "Log.h"

#include <d3dx10math.h>

OrientedBoundBox::OrientedBoundBox(const STriangulationCoordsInfo& in_coordsInfo)
{
	_vPos = vm::Vector3df(in_coordsInfo.vPosition);
	_vXAxis = vm::Vector3df(in_coordsInfo.vXAxis);
	_vYAxis = vm::Vector3df(in_coordsInfo.vYAxis);
	_vZAxis = vm::Vector3df(in_coordsInfo.vZAxis);

	vm::Vector3df vMin = vm::Vector3df(in_coordsInfo.vBoundBoxMinimum);
	vm::Vector3df vMax = vm::Vector3df(in_coordsInfo.vBoundBoxMaximum);

	_vHalfsizes = (vMax - vMin)*0.5;
	vm::Vector3df vPosLocalOffset = (vMax + vMin)*0.5;

	_vPos +=
		vPosLocalOffset[0] * _vXAxis +
		vPosLocalOffset[1] * _vYAxis +
		vPosLocalOffset[2] * _vZAxis
		;
	
}

void OrientedBoundBox::getCornerPoints(vm::Vector3df out_pvCorners[8])
{
	out_pvCorners[0] = _vPos + _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];
	out_pvCorners[1] = _vPos + _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	out_pvCorners[2] = _vPos + _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	out_pvCorners[3] = _vPos + _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];

	out_pvCorners[4] = _vPos - _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];
	out_pvCorners[5] = _vPos - _vXAxis * _vHalfsizes[0] - _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	out_pvCorners[6] = _vPos - _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] + _vZAxis * _vHalfsizes[2];
	out_pvCorners[7] = _vPos - _vXAxis * _vHalfsizes[0] + _vYAxis * _vHalfsizes[1] - _vZAxis * _vHalfsizes[2];
}

vm::Vector3df OrientedBoundBox::projectPoint(const vm::Vector3df& v) const
{
	vm::Vector3df vDelta = v - _vPos;
	return vm::Vector3df(
		vm::dot_prod(vDelta, _vXAxis),
		vm::dot_prod(vDelta, _vYAxis),
		vm::dot_prod(vDelta, _vZAxis)
	);
}

CTerrainObject::CTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
	const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, HeightfieldConverter* in_pHF, AsyncTaskManager* in_pTaskManager)
	: _ID(ID), _params(in_pBlockDesc), _textureFileName(in_wsTextureFileName), _heightmapFileName(in_wsHeightmapFileName), _pHeightfieldConverter(in_pHF),
	_OBB(in_coordsInfo), _pTaskManager(in_pTaskManager)
{
	
}

CTerrainObject::~CTerrainObject()
{
	_pTaskManager->removeTask(_ID, true);

	if (_apObjectVertices)
		delete[] _apObjectVertices;
}


void CTerrainObject::GetBoundBoxCorners(D3DXVECTOR3 out_pvCorners[8]) const
{
	vm::Vector3df vPoints[8];

	OrientedBoundBox OBB = GetOrientedBoundBox();

	OBB.getCornerPoints(vPoints);

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

	_apObjectVertices = new SVertex [(_pTriangulation->nCountLat - 1)*(_pTriangulation->nCountLong - 1) * 6];
		
	if (_pHeightfieldConverter->UnmapTriangulation(_pTriangulation, _apObjectVertices, nullptr))
	{
		CTerrainObject* _this = this;
		_pTaskManager->appendTask(_ID, [=]() {_this->calculatePreciseBoundBox(); });
	}
	else
	{
		LogMessage("Failed to unmap triangulation. ObjectID: %d", _ID);
		delete[] _apObjectVertices;

		_apObjectVertices = nullptr;
	}
}

void CTerrainObject::calculatePreciseBoundBox()
{
	vm::Vector3df vMinPoint(0, 0, 0);
	vm::Vector3df vMaxPoint(0, 0, 0);

	OrientedBoundBox originalBB = _OBB;
	float fWorldScale = _pHeightfieldConverter->GetWorldScale();

	for (unsigned int i = 0; i < _pTriangulation->nVertexCount; i ++)
	{
		const SVertex& vtx = _apObjectVertices[i];
		vm::Vector3df vVertexPos = vm::Vector3df(vtx.position.x, vtx.position.y, vtx.position.z);
		
		vm::Vector3df vProjectedPos = originalBB.projectPoint(vVertexPos);

		bool ignorePoint = false;
		for (int j = 0; j < 3; j++)
		{
			if (fabs(vProjectedPos[j]) > 12000000.0 * fWorldScale)
				ignorePoint = true;
		}

		if (ignorePoint)
			continue;

		for (int j = 0; j < 3; j++)
		{
			if (vProjectedPos[j] < vMinPoint[j]) vMinPoint[j] = vProjectedPos[j];
			if (vProjectedPos[j] > vMaxPoint[j]) vMaxPoint[j] = vProjectedPos[j];
		}
	}

	STriangulationCoordsInfo coordsInfo;
	originalBB._vPos.ToArray(coordsInfo.vPosition);
	originalBB._vXAxis.ToArray(coordsInfo.vXAxis);
	originalBB._vYAxis.ToArray(coordsInfo.vYAxis);
	originalBB._vZAxis.ToArray(coordsInfo.vZAxis);
	vMinPoint.ToArray(coordsInfo.vBoundBoxMinimum);
	vMaxPoint.ToArray(coordsInfo.vBoundBoxMaximum);

	std::lock_guard<std::mutex> obbLock(_obbMutex);
	_OBB = OrientedBoundBox(coordsInfo);
}


vm::Vector3df ComputeTripleNormal(const vm::Vector3df& v0, const vm::Vector3df& v1, const vm::Vector3df& v2)
{
	vm::Vector3df n = vm::normalize(vm::cross(v1 - v0, v2 - v0));
	if (vm::dot_prod(n, v0) < 0)
		n = -n;

	return n;
}

// returns true if in_vPos is above
bool CTerrainObject::CalculateProjectionOnSurface(const vm::Vector3df& in_vPos, vm::Vector3df& out_vProjection, vm::Vector3df& out_vNormal)
{
	double dfLong, dfLat;
	bool isPositionAboveBlock = GetObjectManager()->GetClippedProjection(_ID, in_vPos, dfLat, dfLong);

	if (!_apObjectVertices)
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

	SVertex* pQuadStartVertex = &_apObjectVertices[(iQuadLong * (_pTriangulation->nCountLat - 1) + iQuadLat) * 6];

	quadVertices[0] = pQuadStartVertex[0];
	quadVertices[1] = pQuadStartVertex[1];
	quadVertices[2] = pQuadStartVertex[2];
	quadVertices[3] = pQuadStartVertex[5];

	vm::Vector3df n1 = ComputeTripleNormal(quadVertices[0].pos, quadVertices[1].pos, quadVertices[2].pos);
	vm::Vector3df n2 = ComputeTripleNormal(quadVertices[0].pos, quadVertices[2].pos, quadVertices[3].pos);

	QuadVertex p0 = QuadVertex::lerp(dfQuadLatCoeff, quadVertices[1], quadVertices[0]);
	QuadVertex p1 = QuadVertex::lerp(dfQuadLatCoeff, quadVertices[2], quadVertices[3]);

	QuadVertex result = QuadVertex::lerp(dfQuadLongCoeff, p0, p1);

	out_vProjection = result.pos;
	out_vNormal = vm::normalize(n1 + n2);

	return isPositionAboveBlock;
}

OrientedBoundBox CTerrainObject::GetOrientedBoundBox() const
{
	std::lock_guard<std::mutex> obbLock(_obbMutex);

	return _OBB;
}
