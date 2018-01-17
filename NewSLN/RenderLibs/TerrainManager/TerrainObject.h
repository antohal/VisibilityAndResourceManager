#pragma once

#include "vecmath.h"
#include "TerrainManager.h"
#include "HeightfieldConverter.h"
#include "TerrainObjectManager.h"

#include <vector>
#include <string>
#include <mutex>
#include <thread>

class AsyncTaskManager;

struct OrientedBoundBox
{
	OrientedBoundBox() {}
	OrientedBoundBox(const STriangulationCoordsInfo& in_coordsInfo);

	void			getCornerPoints(vm::Vector3df out_pvCorners[8]);
	vm::Vector3df	projectPoint(const vm::Vector3df& v) const;

	vm::Vector3df	_vPos		= vm::Vector3df(0, 0, 0);
	vm::Vector3df	_vXAxis		= vm::Vector3df(1, 0, 0);
	vm::Vector3df	_vYAxis		= vm::Vector3df(0, 1, 0);
	vm::Vector3df	_vZAxis		= vm::Vector3df(0, 0, 1);
	vm::Vector3df	_vHalfsizes = vm::Vector3df(0, 0, 0);
};

class CTerrainObject
{
public:

	CTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, HeightfieldConverter* in_pHF, AsyncTaskManager* in_pTaskManager);

	~CTerrainObject();

	const TerrainObjectID&	GetID() const {
		return _ID;
	}

	const STerrainBlockParams*	GetParams() const {
		return &_params;
	}

	void SetTextureReady() { _bTextureReady = true; }
	bool IsTriangulationReady() const { return _bTriangulationsReady; }

	void SetTriangulationReady(STriangulation* pTri) 
	{
		_bTriangulationsReady = true;
		_pTriangulation = pTri;

		initVertexBuffer();
	}

	STriangulation* GetTriangulation() {
		return _pTriangulation;
	}

	virtual bool IsDataReady() const {
		return _bTriangulationsReady && _bTextureReady;
	}

	// returns true if in_vPos is above. Position returned in vertex space!
	bool CalculateProjectionOnSurface(const vm::Vector3df& in_vPos, vm::Vector3df& out_vProjection, vm::Vector3df& out_vNormal) const;

	bool CalculateClosestPoint(const vm::Vector3df& in_vPos, vm::Vector3df& out_vPoint, vm::Vector3df& out_vNormal) const;

	void CalculateReferencePoints(std::vector<vm::Vector3df>** out_pvecPoints, std::vector<vm::Vector3df>** out_pvecNormals) const;
	void GetBoundBoxCorners(D3DXVECTOR3 out_pvCorners[8]) const;

	const wchar_t*				GetTextureFileName() const { return _textureFileName.c_str(); }
	const wchar_t*				GetHeightmapFileName() const { return _heightmapFileName.c_str(); }

	OrientedBoundBox			GetOrientedBoundBox() const;

	float						timeSinceUnused = 0;

private:

	void						initVertexBuffer();
	void						calculateVerticesAndPreciseBoundBox();

	struct TerrainVertex
	{
		vm::Vector3f pos, normal;

		TerrainVertex() {}
		TerrainVertex(const SVertex& v) : pos(v.position.x, v.position.y, v.position.z), normal(v.normal.x, v.normal.y, v.normal.z) {}
		TerrainVertex(const vm::Vector3df& p, const vm::Vector3df& n) : pos(p), normal(n) {}

		static TerrainVertex lerp(double k, const TerrainVertex& v0, const TerrainVertex& v1)
		{
			return TerrainVertex(
				v0.pos + k * (v1.pos - v0.pos),
				vm::normalize(v0.normal + k * (v1.normal - v0.normal))
			);
		}
	};

	TerrainObjectID				_ID = -1;
	STerrainBlockParams			_params;

	HeightfieldConverter*		_pHeightfieldConverter = nullptr;
	STriangulation*				_pTriangulation = nullptr;
	AsyncTaskManager*			_pTaskManager = nullptr;

	SVertex*					_apTriangleList = nullptr;
	TerrainVertex*				_apVertices = nullptr;
	bool						_verticesCalculated = false;

	OrientedBoundBox			_OBB;
	mutable std::mutex			_obbMutex;
	//std::thread*				_updateBoundBoxThread = nullptr;

	bool						_bTriangulationsReady = false;
	bool						_bTextureReady = false;

	std::wstring				_textureFileName;
	std::wstring				_heightmapFileName;

	mutable std::vector<vm::Vector3df>	_vecRefPoints;
	mutable std::vector<vm::Vector3df>	_vecRefNormals;
	mutable bool				_bReferencePointsCalulated = false;
};
