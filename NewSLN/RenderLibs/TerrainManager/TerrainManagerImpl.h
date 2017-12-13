#pragma once

#include "TerrainManager.h"
#include "HeightfieldConverter.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "TerrainVisibilityManager.h"

#include "TerrainObjectManager.h"
#include "TerrainVisibility.h"

#include <set>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>

class CInternalTerrainObject  //: public C3DBaseObject
{
public:

	CInternalTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName);

	const TerrainObjectID&	GetID() const {
		return _ID;
	}

	const STerrainBlockParams*	GetParams() const {
		return &_params;
	}

	void InvalidateData() {
		_bTriangulationsReady = false;
		_bTextureReady = false;
	}

	/*void SetDataReady() {
		_bOtherDataReady = true;
	}*/

	void SetTextureReady() {
		_bTextureReady = true;
	}
	
	bool IsTriangulationReady() const {
		return _bTriangulationsReady;
	}

	void SetTriangulationReady(STriangulation* pTri) {
		_bTriangulationsReady = true;
		_pTriangulation = pTri;
	}

	STriangulation* GetTriangulation() {
		return _pTriangulation;
	}

	virtual bool							IsDataReady() const {
		return _bTriangulationsReady && _bTextureReady;
	}

	void CalculateReferencePoints(std::vector<vm::Vector3df>** out_pvecPoints, std::vector<vm::Vector3df>** out_pvecNormals);

	const wchar_t* GetTextureFileName() const {
		return _textureFileName.c_str();
	}

	const wchar_t* GetHeightmapFileName() const {
		return _heightmapFileName.c_str();
	}

	const vm::Vector3df&					GetPos() const { return _vPos; }
	const vm::Vector3df&					GetX() const { return _vXAxis; }
	const vm::Vector3df&					GetY() const { return _vYAxis; }
	const vm::Vector3df&					GetZ() const { return _vZAxis; }
	const vm::Vector3df&					GetHalfSizes() const { return _vHalfsizes; }

	/*const vm::Vector3df&					GetAABBMin() const { return _vAABBMin; }
	const vm::Vector3df&					GetAABBMax() const { return _vAABBMax; }*/

	float						timeSinceUnused = 0;

private:

	TerrainObjectID				_ID = -1;
	STerrainBlockParams			_params;
	
	STriangulation*				_pTriangulation = nullptr;

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

	std::vector<vm::Vector3df>	_vecRefPoints;
	std::vector<vm::Vector3df>	_vecRefNormals;
	bool						_bReferencePointsCalulated = false;
};


class CTerrainManager::CTerrainManagerImpl // : public C3DBaseTerrainObjectManager
{
public:

	CTerrainManagerImpl();
	~CTerrainManagerImpl();

	// �������������. �������� - ��� ����������, ��� ����� ������ �����
	void InitFromDatabaseInfo(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency);

	void SetHeightfieldConverter(HeightfieldConverter*);

	void SetHeightfieldCompressionRatio(unsigned int ratio) {
		_heightfieldCompressionRatio = ratio;
	}

	void SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection);

	// � ������ ������ ���� ������� ����������� 4 ������: 
	// �������, ������� ����� �������
	// �������, ������� ����� ��������
	// �������, ������� ����� ����������
	// �������, ������� ����� �������
	void Update(float in_fDeltaTime);

	// �������� ��� �������� ��� ������� �������
	const wchar_t*	GetTextureFileName(TerrainObjectID ID) const;

	// �������� ��� ����� ����� ��� ������� �������
	const wchar_t*	GetHeightmapFileName(TerrainObjectID ID) const;

	const wchar_t*	GetRootDirectory() const {
		return _pTerrainObjectManager->GetRootDirectory();
	}

	//�������� �������� ������� ����� �� ��������������
	void	GetTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams* out_pParams) const;

	void GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation** out_ppTriangulation);

	void GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

	size_t GetTriangulationsCount() const;

	size_t GetHeightfieldsCount() const;

	//@{ ������ ����� ��������, ������� ����� ������� (����� ����� ��������)
	size_t GetNewObjectsCount() const;
	TerrainObjectID	GetNewObjectID(size_t index) const;
	//@}


	//@{ ������ ��������, ������� ����� ������� (������ �� ������ ������������ �������)
	size_t GetObjectsToDeleteCount() const;
	TerrainObjectID GetObjectToDeleteID(size_t index) const;
	//@}

	//@{ ������ ������� ������� ��������
	size_t GetVisibleObjectsCount() const;
	TerrainObjectID GetVisibleObjectID(size_t index) const;
	//@}

	//@{ ������ �������� ��� ������� ����� ��������� ����� �����
	size_t GetNewHeightmapsCount() const;
	TerrainObjectID GetNewHeightmapObjectID(size_t index) const;
	//@}


	//void SetDataReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap = nullptr);
	void SetTextureReady(TerrainObjectID ID);
	void SetHeightmapReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap);

	void SetAwaitVisibleForDataReady(bool in_bAwait);

	// ��������� - ������ �� ������������ ��� ������� �����
	bool IsTriangulationReady(TerrainObjectID ID) const;

	//@{ ������� ��������� ������� ���������� �����

	// ���������� ������� ���������� ��� NLods �����
	void SetLodDistancesKM(double* aLodDistances, size_t NLods);

	// ������� ������� ���������� ��� NLods �����
	void GetLodDistancesKM(double* aLodDistances, size_t NLods);

	// ���������� ������������� ������� ���������� ������ �� ������������� ���������� �������� �� �������
	// (�����������: FOV ������, ���������� ������, ������ ������� �����, �������� ������� ��������������� ������ �����)
	// ����� ������ ���� �������, �� ��������� ������ Update ���������� �������� ������� �����.
	void CalculateLodDistances(float in_fMaxPixelsPerTexel, unsigned int in_uiScreenResolutionX, unsigned int in_uiScreenResolutionY);


	void SetLastLodDistanceOnSurface(double distance);

	//@}

	//@{ ������� ��������� ���������� ��� ��������

	// ��������� ��������� � ����������� ���������� �����������
	void FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams);

	// ��������� ��������� � ����������� ��� ���������� �����
	void FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams);

	//@}


	bool UpdateTriangulations();

private:


	float GetWorldRadius() const;
	float GetMinCellSize() const;

	CInternalTerrainObject* CreateObject(TerrainObjectID ID);
	void DestroyObject(TerrainObjectID ID);
	void DestroyObjects();

	void ReleaseTriangulationsAndHeightmaps();

	void ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords, unsigned int nLod);

	void CalculateReadyAndVisibleSet();

	bool FillTerrainBlockParams(TerrainObjectID ID, STerrainBlockParams& out_Params) const;
	void UpdateTriangulationsAndHeightfieldLifetime();
	void UpdateObjectsLifetime(float in_fDeltatime);

	void ManageDeadObjects();

	//@{ Main objects
	//---------------------- New mechanism
	CTerrainObjectManager*	_pTerrainObjectManager = nullptr;
	CTerrainVisibility*		_pTerrainVisibility = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;

	HeightfieldConverter*	_pHeightfieldConverter = nullptr;
	//@}

	//@{ Vars
	//TerrainObjectID			_idCurrentIDForNewObject = 0;
	float					_fWorldScale = 1.f;
	float					_fWorldSize = 10000000.f;
	unsigned int			_heightfieldCompressionRatio = 1;

	std::chrono::time_point<std::chrono::steady_clock>	_prevFrameTime;
	//@}

	SGlobalTerrainShaderParams							_globalTerrainShaderParams;

	//@{ Containers
	//std::set<CInternalTerrainObject*>					_setObjects;
	std::map<TerrainObjectID, CInternalTerrainObject*>	_mapId2Object;
	mutable std::mutex									_objectsMutex;

	std::wstring										_wsPlanetRootDirectory;

	mutable std::mutex									_containersMutex;

	//@{ following containers are guarded by mutex (_containersMutex)
	std::vector<TerrainObjectID>						_vecNewObjectIDs;
	std::set<TerrainObjectID>							_setNotReadyTriangulations;
	std::vector<TerrainObjectID>						_vecObjectsToDelete;
	std::set<TerrainObjectID>							_setPreliminaryObjectsToDelete;

	std::set<TerrainObjectID>							_setPreliminaryVisibleObjectIDs;
	std::vector<TerrainObjectID>						_vecReadyVisibleObjects;
	std::set<CInternalTerrainObject*>					_setPreliminaryVisibleObjects;


	std::vector<TerrainObjectID>						_vecHeightmapsToCreate;

	std::set<TerrainObjectID>							_setCachedHFRequest;
	std::set<TerrainObjectID>							_setObjectsToImmediateDelete;
	//@}

	//@}

	struct SObjectTriangulation
	{
		STriangulation	_triangulation;
		double			_timeSinceDead = 0;
		bool			_alive = false;
		bool			_ready = false;
	};

	struct SObjectHeightfield
	{
		SHeightfield	_heightfield;
		double			_timeSinceLastRequest = 0;
		bool			_ready = false;
	};

	SHeightfield*		RequestObjectHeightfield(TerrainObjectID ID);

	mutable std::mutex									_objectTriangulationsMutex;
	std::map<TerrainObjectID, SObjectTriangulation>		_mapObjectTriangulations;

	mutable std::mutex									_objectHeightfieldsMutex;
	std::map<TerrainObjectID, SObjectHeightfield>		_mapObjectHeightfields;

	bool												_bAwaitingVisibleForDataReady = true;

	struct SCameraParams
	{
		vm::Vector3df		vPos = vm::Vector3df(0, 0, 0);
		vm::Vector3df		vDir = vm::Vector3df(1, 0, 0);
		vm::Vector3df		vUp = vm::Vector3df(0, 1, 0);

		D3DMATRIX			mProjection;

		float				fHFovAngleRad = 0, fVFovAngleRad = 0;
		unsigned int		uiScreenResolutionX = 0, uiScreenResolutionY = 0;
	};

	SCameraParams			_cameraParams;

	std::vector<size_t>		_vecLODResolution;
	std::vector<float>		_vecLODDiameter;

	float					_fMaxPixelsPerTexel = 10;
	bool					_bRecalculateLodsDistances = false;
};
