#pragma once

#include "vecmath.h"
#include "TerrainManager.h"
#include "HeightfieldConverter.h"

#include <vector>
#include <string>

class CTerrainObject
{
public:

	CTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, HeightfieldConverter* in_pHF);

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
		calculatePreciseBoundBox();
	}

	STriangulation* GetTriangulation() {
		return _pTriangulation;
	}

	virtual bool							IsDataReady() const {
		return _bTriangulationsReady && _bTextureReady;
	}

	void CalculateReferencePoints(std::vector<vm::Vector3df>** out_pvecPoints, std::vector<vm::Vector3df>** out_pvecNormals) const;
	void GetBoundBoxCorners(D3DXVECTOR3 out_pvCorners[8]) const;

	const wchar_t*				GetTextureFileName() const { return _textureFileName.c_str(); }
	const wchar_t*				GetHeightmapFileName() const { return _heightmapFileName.c_str(); }

	const vm::Vector3df&		GetPos() const { return _vPos; }
	const vm::Vector3df&		GetX() const { return _vXAxis; }
	const vm::Vector3df&		GetY() const { return _vYAxis; }
	const vm::Vector3df&		GetZ() const { return _vZAxis; }
	const vm::Vector3df&		GetHalfSizes() const { return _vHalfsizes; }

	float						timeSinceUnused = 0;

private:

	void						initVertexBuffer();
	void						calculatePreciseBoundBox();

	TerrainObjectID				_ID = -1;
	STerrainBlockParams			_params;

	HeightfieldConverter*		_pHeightfieldConverter = nullptr;
	STriangulation*				_pTriangulation = nullptr;

	SVertex*					_apQuadVertices = nullptr;

	vm::Vector3df				_vPos = vm::Vector3df(0, 0, 0);

	vm::Vector3df				_vXAxis = vm::Vector3df(1, 0, 0);
	vm::Vector3df				_vYAxis = vm::Vector3df(0, 1, 0);
	vm::Vector3df				_vZAxis = vm::Vector3df(0, 0, 1);
	vm::Vector3df				_vHalfsizes = vm::Vector3df(0, 0, 0);

	vm::Vector3df				_vAABBMin = vm::Vector3df(0, 0, 0);
	vm::Vector3df				_vAABBMax = vm::Vector3df(0, 0, 0);

	bool						_bTriangulationsReady = false;
	//bool						_bOtherDataReady = false;
	bool						_bTextureReady = false;

	std::wstring				_textureFileName;
	std::wstring				_heightmapFileName;

	mutable std::vector<vm::Vector3df>	_vecRefPoints;
	mutable std::vector<vm::Vector3df>	_vecRefNormals;
	mutable bool				_bReferencePointsCalulated = false;
};
