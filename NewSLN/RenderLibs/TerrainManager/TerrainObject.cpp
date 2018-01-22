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

CTerrainObject::CTerrainObject(CTerrainObjectOwner* in_pOwner, TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
	const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, HeightfieldConverter* in_pHF, AsyncTaskManager* in_pTaskManager)
	: _pOwner(in_pOwner), _ID(ID), _params(in_pBlockDesc), _textureFileName(in_wsTextureFileName), _heightmapFileName(in_wsHeightmapFileName), _pHeightfieldConverter(in_pHF),
	_OBB(in_coordsInfo), _pTaskManager(in_pTaskManager)
{
	
}

CTerrainObject::~CTerrainObject()
{
	_pTaskManager->removeTask(_ID, true);

	if (_apTriangleList)
		delete[] _apTriangleList;
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

	_apTriangleList = new SVertex [(_pTriangulation->nCountLat - 1)*(_pTriangulation->nCountLong - 1) * 6];
		
	if (_pHeightfieldConverter->UnmapTriangulation(_pTriangulation, _apTriangleList, nullptr))
	{
		CTerrainObject* _this = this;
		_pTaskManager->appendTask(_ID, [=]() {_this->calculateVerticesAndPreciseBoundBox(); });
	}
	else
	{
		LogMessage("Failed to unmap triangulation. ObjectID: %d", _ID);
		delete[] _apTriangleList;

		_apTriangleList = nullptr;
	}
}

void CTerrainObject::calculateVerticesAndPreciseBoundBox()
{
	if (!_apTriangleList || _verticesCalculated)
		return;

	// --- calc vertices ---

	_apVertices = new TerrainVertex[_pTriangulation->nCountLat * _pTriangulation->nCountLong];
	ZeroMemory(_apVertices, sizeof(TerrainVertex)*(_pTriangulation->nCountLat * _pTriangulation->nCountLong));

	auto setVertex = [this](unsigned int iLat, unsigned int iLong, const TerrainVertex& v)
	{
		_apVertices[iLong * (_pTriangulation->nCountLat) + iLat] = v;
	};

	struct SQuad
	{
		vm::Vector3f vertices[4];
		vm::Vector3f normals[4];
	};

	auto dxToVec = [](const D3DXVECTOR3& v) -> vm::Vector3f
	{
		return vm::Vector3f(v.x, v.y, v.z);
	};

	auto getQuad = [&](unsigned int iQuadLat, unsigned int iQuadLong) ->SQuad
	{
		unsigned int iQuadStartVertex = (iQuadLong * (_pTriangulation->nCountLat - 1) + iQuadLat) * 6;

		SQuad quad;
		quad.vertices[0] = dxToVec(_apTriangleList[iQuadStartVertex].position);
		quad.vertices[1] = dxToVec(_apTriangleList[iQuadStartVertex + 1].position);
		quad.vertices[2] = dxToVec(_apTriangleList[iQuadStartVertex + 2].position);
		quad.vertices[3] = dxToVec(_apTriangleList[iQuadStartVertex + 5].position);

		quad.normals[0] = dxToVec(_apTriangleList[iQuadStartVertex].normal);
		quad.normals[1] = dxToVec(_apTriangleList[iQuadStartVertex + 1].normal);
		quad.normals[2] = dxToVec(_apTriangleList[iQuadStartVertex + 2].normal);
		quad.normals[3] = dxToVec(_apTriangleList[iQuadStartVertex + 5].normal);

		return quad;
	};

	auto computeMeanNormal = [&](const vm::Vector3f& pos, const vm::Vector3f neighbours[4]) -> vm::Vector3f
	{
		return - vm::normalize(
			vm::cross(neighbours[0] - pos, neighbours[3] - pos) +
			vm::cross(neighbours[3] - pos, neighbours[2] - pos) +
			vm::cross(neighbours[2] - pos, neighbours[1] - pos) +
			vm::cross(neighbours[1] - pos, neighbours[0] - pos)
		);
	};

	for (unsigned int iQuadLat = 0; iQuadLat < _pTriangulation->nCountLat - 1; iQuadLat++)
	{
		for (unsigned int iQuadLong = 0; iQuadLong < _pTriangulation->nCountLong - 1; iQuadLong++)
		{
			SQuad quad = getQuad(iQuadLat, iQuadLong);

			if (iQuadLat > 0 && iQuadLong > 0)
			{
				SQuad leftNeighbour = getQuad(iQuadLat - 1, iQuadLong),
					bottomNeighbour = getQuad(iQuadLat, iQuadLong - 1);

				vm::Vector3f neighbourVertices[4] = { quad.vertices[0], quad.vertices[2], leftNeighbour.vertices[1], bottomNeighbour.vertices[1] };

				setVertex(iQuadLat, iQuadLong, TerrainVertex(quad.vertices[1], 
					computeMeanNormal(quad.vertices[1], neighbourVertices )));
			}
			else
			{
				setVertex(iQuadLat, iQuadLong, TerrainVertex(quad.vertices[1], quad.normals[1]));
			}

			if (iQuadLat == _pTriangulation->nCountLat - 2 && iQuadLong == _pTriangulation->nCountLong - 2)
			{
				setVertex(iQuadLat, iQuadLong + 1, TerrainVertex(quad.vertices[2], quad.normals[2]));
				setVertex(iQuadLat + 1, iQuadLong + 1, TerrainVertex(quad.vertices[3], quad.normals[3]));
				setVertex(iQuadLat + 1, iQuadLong, TerrainVertex(quad.vertices[0], quad.normals[0]));
			}
			else if (iQuadLat == _pTriangulation->nCountLat - 2)
				setVertex(iQuadLat + 1, iQuadLong, TerrainVertex(quad.vertices[0], quad.normals[0]));
			else if (iQuadLong == _pTriangulation->nCountLong - 2)
				setVertex(iQuadLat, iQuadLong + 1, TerrainVertex(quad.vertices[2], quad.normals[2]));
		}
	}

	_verticesCalculated = true;

	delete[] _apTriangleList;
	_apTriangleList = nullptr;

	// --- calc bound box ---

	vm::Vector3df vMinPoint(0, 0, 0);
	vm::Vector3df vMaxPoint(0, 0, 0);

	OrientedBoundBox originalBB = _OBB;
	float fWorldScale = _pHeightfieldConverter->GetWorldScale();

	double sumHeight = 0;

	for (unsigned int i = 0; i < _pTriangulation->nCountLat * _pTriangulation->nCountLong; i ++)
	{
		const TerrainVertex& vtx = _apVertices[i];
		vm::Vector3df vVertexPos = vtx.pos;
		
		vm::Vector3df vProjectedPos = originalBB.projectPoint(vVertexPos);

		sumHeight += GetWGS84Height(vVertexPos / fWorldScale);

		for (int j = 0; j < 3; j++)
		{
			if (vProjectedPos[j] < vMinPoint[j]) vMinPoint[j] = vProjectedPos[j];
			if (vProjectedPos[j] > vMaxPoint[j]) vMaxPoint[j] = vProjectedPos[j];
		}
	}

	double midHeight = sumHeight / (_pTriangulation->nCountLat * _pTriangulation->nCountLong);

	_pOwner->CacheTerrainObjectMidHeight(_ID, midHeight);

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

// returns true if in_vPos is above
bool CTerrainObject::CalculateProjectionOnSurface(const vm::Vector3df& in_vPos, vm::Vector3df& out_vProjection, vm::Vector3df& out_vNormal) const
{
	double dfLong, dfLat;
	bool isPositionAboveBlock = GetObjectManager()->GetClippedProjection(_ID, in_vPos, dfLat, dfLong);

	if (!_verticesCalculated)
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

	TerrainVertex quadVertices[4];

	auto getVertex = [this](unsigned int iLat, unsigned int iLong) -> const TerrainVertex&
	{
		return _apVertices[iLong * (_pTriangulation->nCountLat) + iLat];
	};

	quadVertices[0] = getVertex(iQuadLat + 1, iQuadLong);
	quadVertices[1] = getVertex(iQuadLat, iQuadLong);
	quadVertices[2] = getVertex(iQuadLat, iQuadLong + 1);
	quadVertices[3] = getVertex(iQuadLat + 1, iQuadLong + 1);

	TerrainVertex p0 = TerrainVertex::lerp(dfQuadLatCoeff, quadVertices[1], quadVertices[0]);
	TerrainVertex p1 = TerrainVertex::lerp(dfQuadLatCoeff, quadVertices[2], quadVertices[3]);

	TerrainVertex result = TerrainVertex::lerp(dfQuadLongCoeff, p0, p1);

	out_vProjection = result.pos;
	out_vNormal = result.normal;

	return isPositionAboveBlock;
}

OrientedBoundBox CTerrainObject::GetOrientedBoundBox() const
{
	std::lock_guard<std::mutex> obbLock(_obbMutex);
	return _OBB;
}


bool CTerrainObject::CalculateClosestPoint(const vm::Vector3df& in_vPos, vm::Vector3df& out_vPoint, vm::Vector3df& out_vNormal) const
{
	const unsigned int N_ITERATIONS_MAX = 20;
	const float fGradientCoeff = 5.f;

	vm::Vector3df vProjection, vProjectionNormal;

	bool isPositionAboveBlock = CalculateProjectionOnSurface(in_vPos, vProjection, vProjectionNormal);

	float fProjectionDist = vm::length(vm::Vector3f(in_vPos) - vm::Vector3f(vProjection));

	out_vPoint = vProjection;
	out_vNormal = vProjectionNormal;

	if (!_verticesCalculated)
		return isPositionAboveBlock;
	
	float fCurrentDist = vm::length(vm::Vector3f(in_vPos) - vm::Vector3f(out_vPoint));

	vm::Vector3f vStartPos = in_vPos;
	vm::Vector3f vResultPos = out_vPoint, vResultNormal = out_vNormal;

	unsigned int uiAccuracy = _pOwner->GetMinDistAlgorithmAccuracy();

	for (unsigned int i = 0; i < _pTriangulation->nCountLat * _pTriangulation->nCountLong; i+= uiAccuracy)
	{
		float fIterDist = vm::length(_apVertices[i].pos - vStartPos);
		
		if (fIterDist < fCurrentDist)
		{
			fCurrentDist = fIterDist;
			vResultPos = _apVertices[i].pos;
			vResultNormal = _apVertices[i].normal;
		}
	}

	if (fCurrentDist < fProjectionDist)
	{
		out_vPoint = vResultPos;
		out_vNormal = vResultNormal;
	}

	return isPositionAboveBlock;
}
