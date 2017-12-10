#include <Shlwapi.h>

#include "TerrainManagerImpl.h"
#include "Log.h"
#include "FileUtil.h"
#include "wgs84.h"

#include <d3dx10math.h>
#include <algorithm>

#include "TerrainVisibilityManagerImpl.h"
//#include "Geometry/InFrustum.h"
#include "Geometry/Vector3D.h"
#include "Geometry/Matrix.h"


#define USE_ENGINE_SCALE

#ifdef USE_ENGINE_SCALE
const float g_fMasterScale = 100.f;
#else
const float g_fMasterScale = 1.f;
#endif

const float g_fMaxHFAliveTime = 2.f;

CInternalTerrainObject::CInternalTerrainObject(TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
	const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName)
	: _ID(ID), _params(in_pBlockDesc), _textureFileName(in_wsTextureFileName), _heightmapFileName(in_wsHeightmapFileName)
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

//---------------------------------------------------------------------------------------------------
//	CInternalTerrainObject
//---------------------------------------------------------------------------------------------------

CTerrainManager::CTerrainManager()
{
	LogInit("TerrainManager.log");
	LogEnable(true);

	_implementation = new CTerrainManagerImpl;
}

CTerrainManager::~CTerrainManager()
{
	delete _implementation;
}
// �������������. �������� - ��� ����������, ��� ����� ������ �����
//void CTerrainManager::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff)
//{
//	_implementation->Init(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, in_fWorldScale, in_fWorldSize, in_fLongitudeScaleCoeff, in_fLattitudeScaleCoeff);
//}

void CTerrainManager::InitFromDatabaseInfo(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency)
{
	_implementation->InitFromDatabaseInfo(in_pD3DDevice11, in_pDeviceContext, in_pcwszFileName, in_uiMaxDepth, in_fWorldScale, in_fWorldSize, in_bCalculateAdjacency);
}

void CTerrainManager::SetHeightfieldConverter(HeightfieldConverter * p)
{
	_implementation->SetHeightfieldConverter(p);
}

void CTerrainManager::SetHeightfieldCompressionRatio(unsigned int ratio)
{
	_implementation->SetHeightfieldCompressionRatio(ratio);
}

//void CTerrainManager::InitGenerated(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize)
//{
//	_implementation->InitGenerated(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, N, M, depth, in_fWorldScale, in_fWorldSize);
//}

void CTerrainManager::SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection)
{
	_implementation->SetViewProjection(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

//@{ ������� ��������� ���������� ��� ��������

// ��������� ��������� � ����������� ���������� �����������
void CTerrainManager::FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams)
{
	_implementation->FillGlobalShaderParams(out_pGlobalShaderParams);
}

// ��������� ��������� � ����������� ��� ���������� �����
void CTerrainManager::FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams)
{
	_implementation->FillTerrainBlockShaderParams(ID, out_pTerrainBlockShaderParams);
}

//@}

// � ������ ������ ���� ������� ����������� 4 ������: 
// �������, ������� ����� �������
// �������, ������� ����� ��������
// �������, ������� ����� ����������
// �������, ������� ����� �������
void CTerrainManager::Update(float in_fDeltaTime)
{
	_implementation->Update(in_fDeltaTime);
}

bool CTerrainManager::UpdateTriangulations()
{
	return _implementation->UpdateTriangulations();
}

const wchar_t * CTerrainManager::GetRootDirectory() const
{
	return _implementation->GetRootDirectory();
}

// �������� ��� �������� ��� ������� �������
const wchar_t*	CTerrainManager::GetTextureFileName(TerrainObjectID ID) const
{
	return _implementation->GetTextureFileName(ID);
}

// �������� ��� ����� ����� ��� ������� �������
const wchar_t*	CTerrainManager::GetHeightmapFileName(TerrainObjectID ID) const
{
	return _implementation->GetHeightmapFileName(ID);
}


//�������� �������� ������� ����� �� ��������������
void	CTerrainManager::GetTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams* out_pParams) const
{
	_implementation->GetTerrainObjectParams(ID, out_pParams);
}

void CTerrainManager::GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation ** out_ppTriangulation) const
{
	_implementation->GetTerrainObjectTriangulation(ID, out_ppTriangulation);
}

void CTerrainManager::GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8])
{
	_implementation->GetTerrainObjectNeighbours(ID, outNeighbours);
}

size_t CTerrainManager::GetTriangulationsCount() const
{
	return _implementation->GetTriangulationsCount();
}

size_t CTerrainManager::GetHeightfieldsCount() const
{
	return _implementation->GetHeightfieldsCount();
}

//@{ ������ ����� ��������, ������� ����� ������� (����� ����� ��������)
size_t CTerrainManager::GetNewObjectsCount() const
{
	return _implementation->GetNewObjectsCount();
}

TerrainObjectID	CTerrainManager::GetNewObjectID(size_t index) const
{
	return _implementation->GetNewObjectID(index);
}
//@}


//@{ ������ ��������, ������� ����� ������� (������ �� ������ ������������ �������)
size_t CTerrainManager::GetObjectsToDeleteCount() const
{
	return _implementation->GetObjectsToDeleteCount();
}

TerrainObjectID CTerrainManager::GetObjectToDeleteID(size_t index) const
{
	return _implementation->GetObjectToDeleteID(index);
}

//@}

//@{ ������ ������� ������� ��������
size_t CTerrainManager::GetVisibleObjectsCount() const
{
	return _implementation->GetVisibleObjectsCount();
}

TerrainObjectID CTerrainManager::GetVisibleObjectID(size_t index) const
{
	return _implementation->GetVisibleObjectID(index);
}

//void CTerrainManager::SetDataReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap)
//{
//	_implementation->SetDataReady(ID, in_pLoadedHeightmap);
//}

void CTerrainManager::SetTextureReady(TerrainObjectID ID)
{
	_implementation->SetTextureReady(ID);
}

void CTerrainManager::SetHeightmapReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap)
{
	_implementation->SetHeightmapReady(ID, in_pLoadedHeightmap);
}

bool CTerrainManager::IsTriangulationReady(TerrainObjectID ID) const
{
	return _implementation->IsTriangulationReady(ID);
}

void CTerrainManager::SetAwaitVisibleForDataReady(bool in_bAwait)
{
	_implementation->SetAwaitVisibleForDataReady(in_bAwait);
}

void CTerrainManager::SetLodDistancesKM(double * aLodDistances, size_t NLods)
{
	_implementation->SetLodDistancesKM(aLodDistances, NLods);
}

void CTerrainManager::GetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	_implementation->GetLodDistancesKM(aLodDistances, NLods);
}

void CTerrainManager::CalculateLodDistances(float in_fMaxPixelsPerTexel, unsigned int in_uiScreenResolutionX, unsigned int in_uiScreenResolutionY)
{
	_implementation->CalculateLodDistances(in_fMaxPixelsPerTexel, in_uiScreenResolutionX, in_uiScreenResolutionY);
}

void CTerrainManager::SetLastLodDistanceOnSurface(double distance)
{
	_implementation->SetLastLodDistanceOnSurface(distance);
}

//@{ ������ �������� ��� ������� ����� ��������� ����� �����

size_t CTerrainManager::GetNewHeightmapsCount() const
{
	return _implementation->GetNewHeightmapsCount();
}

TerrainObjectID CTerrainManager::GetNewHeightmapObjectID(size_t index) const
{
	return _implementation->GetNewHeightmapObjectID(index);
}

//@}


//@}


// �������� ��������� �� �������� �������� (���� ���������� ������ ���������� ������������� ��������� ��������, �������� �� �������� ��-���������)
//CResourceManager* CTerrainManager::GetResourceManager()
//{
//	return _implementation->GetResourceManager();
//}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//		CTerrainManager::CTerrainManagerImpl
//-------------------------------------------------------------------------------------------------------------------------------------------------------

CTerrainManager::CTerrainManagerImpl::CTerrainManagerImpl()
{
	_vecLODResolution.resize(20);
	_vecLODDiameter.resize(20);

	for (size_t i = 0; i < 20; i++)
	{
		_vecLODDiameter[i] = 0;
		_vecLODResolution[i] = 512;
	}

	const double Rmin = 6356752.3142;
	_vecLODDiameter[0] = (float) (2 * Rmin);

	_pTerrainObjectManager = new CTerrainObjectManager;
}

CTerrainManager::CTerrainManagerImpl::~CTerrainManagerImpl()
{
	DestroyObjects();
	ReleaseTriangulationsAndHeightmaps();

	if (_pTerrainObjectManager)
		delete _pTerrainObjectManager;

	if (_pTerrainVisibility)
		delete _pTerrainVisibility;

	if (_pVisibilityManager)
		delete _pVisibilityManager;
}

void CTerrainManager::CTerrainManagerImpl::InitFromDatabaseInfo(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency)
{
	ZeroMemory(&_globalTerrainShaderParams, sizeof(SGlobalTerrainShaderParams));

	_fWorldSize = in_fWorldSize;
	_fWorldScale = g_fMasterScale*in_fWorldScale;

	LogMessage("Loading planet terrain info");

	std::wstring wsDbFileName = in_pcwszFileName;

	unsigned int uiResultingMaxDepth = 0;
	bool bSuccessifulRead = _pTerrainObjectManager->LoadDatabaseFile(in_pcwszFileName, in_uiMaxDepth, uiResultingMaxDepth);


	if (!bSuccessifulRead)
	{
		LogMessage("Error reading earth database file %ls, aborting", in_pcwszFileName);
		return;
	}
	else
	{

		double dfCurrentLatDiam = M_PI;
		double Rmax = 6356752.3142;

		for (int i = 0; i < _pTerrainObjectManager->GetDatabaseInfo().LodCount; i++)
		{
			dfCurrentLatDiam /= _pTerrainObjectManager->GetLodInfos()[i].CountY;

			_vecLODResolution[i] = std::max<short>(_pTerrainObjectManager->GetLodInfos()[i].Width, _pTerrainObjectManager->GetLodInfos()[i].Height) / 2;
			_vecLODDiameter[i] = (float)(Rmax*sqrt(2*(1 - cos(dfCurrentLatDiam))));

			_globalTerrainShaderParams.aVertexCounts[i][0] = _pTerrainObjectManager->GetLodInfos()[i].Width;
			_globalTerrainShaderParams.aVertexCounts[i][1] = _pTerrainObjectManager->GetLodInfos()[i].Height;

			_globalTerrainShaderParams.aPartitionCoefficients[i][0] = _pTerrainObjectManager->GetLodInfos()[i].CountX;
			_globalTerrainShaderParams.aPartitionCoefficients[i][1] = _pTerrainObjectManager->GetLodInfos()[i].CountY;
		}

	}

	unsigned int uiMaxDepth = std::min<unsigned int>(_pTerrainObjectManager->GetDatabaseInfo().LodCount, in_uiMaxDepth);

	if (in_uiMaxDepth == 0)
		uiMaxDepth = _pTerrainObjectManager->GetDatabaseInfo().LodCount;

	_globalTerrainShaderParams.uiLevelsCount = uiMaxDepth;

	// TODO: Read lods structure

	_wsPlanetRootDirectory = ExtractFileDirectory(wsDbFileName);

	_pTerrainVisibility = new CTerrainVisibility(_pTerrainObjectManager, _fWorldScale, 6000000.0f, 0.5, uiResultingMaxDepth);

	_pTerrainVisibility->SetNewObjectHandler([this](TerrainObjectID ID) {
		CreateObject(ID);
	});

	_pTerrainVisibility->SetDeleteObjectHandler([this](TerrainObjectID ID) {
		DestroyObject(ID);
	});

	_pVisibilityManager = new CVisibilityManager(nullptr, _fWorldScale * 20000000.0f, 1000 * _fWorldScale);
}

void CTerrainManager::CTerrainManagerImpl::SetHeightfieldConverter(HeightfieldConverter * in_pHeightfieldConverter)
{
	_pHeightfieldConverter = in_pHeightfieldConverter;
}


static Vector3f ToVec3(const Vector3& v)
{
	return Vector3f(v.x, v.y, v.z);
}

static Vector3 FromVec3(const Vector3f& v)
{
	return Vector3(v.x, v.y, v.z);
}

void CTerrainManager::CTerrainManagerImpl::SetViewProjection(const D3DXVECTOR3 * in_vPos, const D3DXVECTOR3 * in_vDir, const D3DXVECTOR3 * in_vUp, const D3DMATRIX * in_pmProjection)
{
	Vector3 v3Pos, v3Dir, v3Up;

	v3Pos.x = (float)in_vPos->x;
	v3Pos.y = (float)in_vPos->y;
	v3Pos.z = (float)in_vPos->z;
	 
	v3Dir.x = (float)in_vDir->x;
	v3Dir.y = (float)in_vDir->y;
	v3Dir.z = (float)in_vDir->z;
	 
	v3Up.x = (float)in_vUp->x;
	v3Up.y = (float)in_vUp->y;
	v3Up.z = (float)in_vUp->z;


	if (in_pmProjection)
		_cameraParams.mProjection = *in_pmProjection;

	_cameraParams.vPos = vm::Vector3df(v3Pos.x / _fWorldScale, v3Pos.y / _fWorldScale, v3Pos.z / _fWorldScale);
	_cameraParams.vDir = vm::Vector3df(v3Dir.x, v3Dir.y, v3Dir.z);
	_cameraParams.vUp = vm::Vector3df(v3Up.x, v3Up.y, v3Up.z);


	if (_pVisibilityManager)
		_pVisibilityManager->SetViewProjection(v3Pos, v3Dir, v3Up, const_cast<D3DMATRIX *>(in_pmProjection));

	if (_pVisibilityManager)
		_pVisibilityManager->GetFOVAnglesDeg(_cameraParams.fHFovAngleRad, _cameraParams.fVFovAngleRad);

	_cameraParams.fHFovAngleRad *= D2R;
	_cameraParams.fVFovAngleRad *= D2R;
}


SHeightfield*	CTerrainManager::CTerrainManagerImpl::RequestObjectHeightfield(TerrainObjectID ID)
{
	SHeightfield*	pHeightfield = nullptr;

	{
		std::lock_guard<std::mutex> hfLock(_objectHeightfieldsMutex);

		auto it = _mapObjectHeightfields.find(ID);
		if (it != _mapObjectHeightfields.end())
		{
			it->second._timeSinceLastRequest = 0;

			if (it->second._ready)
				pHeightfield = &it->second._heightfield;
		}
		else
		{
			_mapObjectHeightfields[ID] = SObjectHeightfield();

			_vecHeightmapsToCreate.push_back(ID);
		}

	} // unlock mutex

	if (!pHeightfield)
		return nullptr;

	//std::wstring wsHeightmapFileName = _pTerrainObjectManager->GetHeighmapFileName(ID); 

	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params);

	auto objRes = _pTerrainObjectManager->GetObjectHfResolution(ID, _heightfieldCompressionRatio);

	pHeightfield->Config.nCountX = objRes.first;
	pHeightfield->Config.nCountY = objRes.second;

	// ������� ������ ����� ����� �� �����
	//_pHeightfieldConverter->ReadHeightfieldDataFromTexture(wsHeightmapFileName.c_str(), *pHeightfield, (unsigned short)_heightfieldCompressionRatio);

	//LogMessage("Loading faceset. Triangulating heightmap '%ls'", wsHeightmapFileName.c_str());

	// �������� ��������� ������
	pHeightfield->Config.Coords.fMinLattitude = params.fMinLattitude;
	pHeightfield->Config.Coords.fMaxLattitude = params.fMaxLattitude;
	pHeightfield->Config.Coords.fMinLongitude = params.fMinLongitude;
	pHeightfield->Config.Coords.fMaxLongitude = params.fMaxLongitude;

	pHeightfield->fLattitudeCutCoeff = params.fLattitudeCutCoeff;
	pHeightfield->fLongitudeCutCoeff = params.fLongitude�utCoeff;

	return pHeightfield;
}

void CTerrainManager::CTerrainManagerImpl::Update(float in_fDeltaTime)
{

	if (_bRecalculateLodsDistances && _cameraParams.fVFovAngleRad >= 30 * D2R && _cameraParams.uiScreenResolutionY > 0)
	{
		_pTerrainVisibility->CalculateLodDistances(_cameraParams.fVFovAngleRad, _vecLODResolution, _vecLODDiameter,
			_cameraParams.uiScreenResolutionY, _fMaxPixelsPerTexel);

		_bRecalculateLodsDistances = false;
	}

	_vecNewObjectIDs.clear();
	_vecObjectsToDelete.clear();

	if (_pTerrainVisibility)
		_pTerrainVisibility->UpdateObjectsVisibility(in_fDeltaTime, _cameraParams.vPos * _fWorldScale);

	
	static std::set<CInternalTerrainObject*> setVisObjs;
	setVisObjs.clear();

	bool bAllReady = true;

	if (!_pTerrainVisibility)
		return;

	for (TerrainObjectID visID : _pTerrainVisibility->GetVisibleObjects())
	{

		CInternalTerrainObject* pTerrainObject = nullptr;
		{
			std::lock_guard<std::mutex> objLock(_objectsMutex);
			pTerrainObject = _mapId2Object[visID];
		}


		if (!pTerrainObject)
		{
			LogMessage("TerrainManager error: no object created for ID %d", visID);
			continue;
		}

		if (pTerrainObject->IsDataReady() || !_bAwaitingVisibleForDataReady)
		{
			setVisObjs.insert(pTerrainObject);
		}
		else
		{
			bAllReady = false;
			break;
		}
	}


	if (bAllReady)
	{
		_setPreliminaryVisibleObjects = setVisObjs;
		_setPreliminaryVisibleObjectIDs = _pTerrainVisibility->GetVisibleObjects();

		_vecObjectsToDelete = _vecPreliminaryObjectsToDelete;
		_vecPreliminaryObjectsToDelete.clear();

		for (TerrainObjectID deadObj : _vecObjectsToDelete)
		{

			{
				std::lock_guard<std::mutex> lock(_objectTriangulationsMutex);

				auto itTri = _mapObjectTriangulations.find(deadObj);
				if (itTri == _mapObjectTriangulations.end())
				{
					LogMessage("Object %d has no triangulation while was alive", deadObj);
					continue;
				}

				itTri->second._alive = false;
			}

			{
				std::lock_guard<std::mutex> objectsLock(_objectsMutex);

				auto it = _mapId2Object.find(deadObj);
				if (it != _mapId2Object.end())
				{
					CInternalTerrainObject* pObject = it->second;

					delete pObject;

					_mapId2Object.erase(it);
				}
			}

		}
	}

	CalculateReadyAndVisibleSet();
}

void CInternalTerrainObject::CalculateReferencePoints(std::vector<vm::Vector3df>** out_pvecPoints, std::vector<vm::Vector3df>** out_pvecNormals)
{
	if (_bReferencePointsCalulated)
	{
		*out_pvecPoints = &_vecRefPoints;
		*out_pvecNormals = &_vecRefNormals;

		return;
	}

	_vecRefPoints.clear();
	_vecRefNormals.clear();

	double dfMinLat =  _params.fMinLattitude;
	double dfMaxLat =  _params.fMaxLattitude;
	double dfMinLong = _params.fMinLongitude;
	double dfMaxLong = _params.fMaxLongitude;

	double dfDeltaLat = (dfMaxLat - dfMinLat) / 10;
	double dfDeltaLong = (dfMaxLong - dfMinLong) / 10;

	for (double dfLat = dfMinLat; dfLat <= dfMaxLat; dfLat += dfDeltaLat)
	{
		for (double dfLong = dfMinLong; dfLong <= dfMaxLong; dfLong += dfDeltaLong)
		{
			vm::Vector3df vPoint = GetWGS84SurfacePoint(dfLong, dfLat);
			vm::Vector3df vNormal = GetWGS84SurfaceNormal(dfLong, dfLat);

			_vecRefPoints.push_back(vPoint);
			_vecRefNormals.push_back(vNormal);
		}
	}

	*out_pvecPoints = &_vecRefPoints;
	*out_pvecNormals = &_vecRefNormals;

	_bReferencePointsCalulated = true;
}

Vector3 ToVec3(const vm::Vector3df& v)
{
	return Vector3((float)v[0], (float)v[1], (float)v[2]);
}

void CTerrainManager::CTerrainManagerImpl::CalculateReadyAndVisibleSet()
{
	_vecReadyVisibleObjects.clear();

	std::vector<vm::Vector3df>* vecRefPoints = nullptr;
	std::vector<vm::Vector3df>* vecRefNormals = nullptr;

	for (CInternalTerrainObject* pObj : _setPreliminaryVisibleObjects)
	{
		pObj->CalculateReferencePoints(&vecRefPoints, &vecRefNormals);

		if (!_pVisibilityManager->CheckOBBInCamera(ToVec3(pObj->GetPos()), ToVec3(pObj->GetX()), ToVec3(pObj->GetY()), ToVec3(pObj->GetZ()), ToVec3(pObj->GetHalfSizes())))
			continue;

	//	if (!_pVisibilityManager->CheckAABBInCamera(ToVec3(pObj->GetAABBMin()), ToVec3(pObj->GetAABBMax())))
	//		continue;

		// check backfaces
		bool bFullBackSided = true;

		for (size_t i = 0; i < (*vecRefNormals).size(); i++)
		{
			if (vm::dot_prod(normalize((*vecRefPoints)[i] - _cameraParams.vPos), normalize((*vecRefPoints)[i])) < 0.2)
			{
				bFullBackSided = false;
				break;
			}
		}

		if (!bFullBackSided)
			_vecReadyVisibleObjects.push_back(pObj->GetID());
	}
}

size_t CTerrainManager::CTerrainManagerImpl::GetTriangulationsCount() const
{
	std::lock_guard<std::mutex> lock(_objectTriangulationsMutex);
	return _mapObjectTriangulations.size();
}

size_t CTerrainManager::CTerrainManagerImpl::GetHeightfieldsCount() const
{
	std::lock_guard<std::mutex> lock(_objectHeightfieldsMutex);
	return _mapObjectHeightfields.size();
}


void CTerrainManager::CTerrainManagerImpl::UpdateTriangulationsAndHeightfieldLifetime()
{
	if (!_pHeightfieldConverter)
		return;


	std::chrono::time_point<std::chrono::steady_clock> thisFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = thisFrameTime - _prevFrameTime;

	double deltaTime = elapsed.count() / 1000.0;

	if (deltaTime > 1)
		deltaTime = 1;

	_prevFrameTime = thisFrameTime;

	{
		std::lock_guard<std::mutex> hfLock(_objectHeightfieldsMutex);
		for (auto it = _mapObjectHeightfields.begin(); it != _mapObjectHeightfields.end();)
		{
			if (it->second._timeSinceLastRequest > g_fMaxHFAliveTime)
			{
				_pHeightfieldConverter->ReleaseHeightfield(&it->second._heightfield);

				it = _mapObjectHeightfields.erase(it);
			}
			else
			{
				it->second._timeSinceLastRequest += deltaTime;
				it++;
			}
		}
	}


	{
		std::lock_guard<std::mutex> triLock(_objectTriangulationsMutex);
		for (auto it = _mapObjectTriangulations.begin(); it != _mapObjectTriangulations.end();)
		{
			if (it->second._alive)
			{
				it++;
				continue;
			}

			if (it->second._timeSinceDead > g_fMaxHFAliveTime)
			{
				_pHeightfieldConverter->ReleaseTriangulation(&it->second._triangulation);
				it = _mapObjectTriangulations.erase(it);
			}
			else
			{
				if (!it->second._alive)
					it->second._timeSinceDead += deltaTime;

				it++;
			}
		}
	}
}


bool CTerrainManager::CTerrainManagerImpl::UpdateTriangulations()
{
	_vecHeightmapsToCreate.clear();

	if (!_pHeightfieldConverter)
		return false;

	static std::vector<std::pair<TerrainObjectID, SObjectTriangulation*>> s_vecTriangulationsToCreate;
	s_vecTriangulationsToCreate.clear();

	// lock
	{
		std::lock_guard<std::mutex> contLock(_containersMutex);

		for (auto it = _setNotReadyTriangulations.begin(); it != _setNotReadyTriangulations.end(); )
		{
			TerrainObjectID ID = *it;
			CInternalTerrainObject* pInternalObject = nullptr;

			{
				std::lock_guard<std::mutex> objLock(_objectsMutex);
				pInternalObject = _mapId2Object[ID];
			}

			if (!pInternalObject)
			{
				LogMessage("Creating triangulation for deleted object id: %d, removing", ID);

				it = _setNotReadyTriangulations.erase(it);

				continue;
			}


			std::lock_guard<std::mutex> triLock(_objectTriangulationsMutex);

			auto itExisting = _mapObjectTriangulations.find(ID);
			if (itExisting != _mapObjectTriangulations.end() && itExisting->second._ready)
			{
				itExisting->second._alive = true;
				itExisting->second._timeSinceDead = 0;

				pInternalObject->SetTriangulationReady(&itExisting->second._triangulation);

				it = _setNotReadyTriangulations.erase(it);

				continue;
			}

			SObjectTriangulation& objTri = _mapObjectTriangulations[ID];

			objTri._alive = true;
			objTri._timeSinceDead = 0;

			s_vecTriangulationsToCreate.push_back(std::make_pair(ID, &objTri));

			it++;
		}

	}

	STerrainBlockParams params;

	for (std::pair<TerrainObjectID, SObjectTriangulation*>& pt : s_vecTriangulationsToCreate)
	{
		TerrainObjectID ID = pt.first;
		SObjectTriangulation& objTri = *(pt.second);

		_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params);

		bool someNotReadyHF = false;

		SHeightfield* pHeightfield = RequestObjectHeightfield(ID);

		if (!pHeightfield)
			someNotReadyHF = true;

		TerrainObjectID neighbours[8];
		GetTerrainObjectNeighbours(ID, neighbours);

		const SHeightfield* neighbourHeightfields[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		for (int i = 0; i < 8; i++)
		{
			if (neighbours[i] != INVALID_TERRAIN_OBJECT_ID)
			{
				neighbourHeightfields[i] = RequestObjectHeightfield(neighbours[i]);

				if (!neighbourHeightfields[i])
				{
					someNotReadyHF = true;
					//break;
				}
			}
		}

		if (someNotReadyHF)
			continue;

		// �������� ������������ � ������� ComputeShader. � ������� _triangulation ����� ��������� � ���������� ������
		_pHeightfieldConverter->CreateTriangulationImmediate(pHeightfield, params.fLongitude�utCoeff, params.fLattitudeCutCoeff, &objTri._triangulation, neighbourHeightfields);

		objTri._ready = true;

		std::lock_guard<std::mutex> objLock(_objectsMutex);
		auto it = _mapId2Object.find(ID);

		if (it != _mapId2Object.end())
			it->second->SetTriangulationReady(&objTri._triangulation);
		else
			objTri._alive = false;
	}


	UpdateTriangulationsAndHeightfieldLifetime();

	return true;
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams* out_pParams) const
{
	std::lock_guard<std::mutex> lock(_objectsMutex);

	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		_pTerrainObjectManager->ComputeTerrainObjectParams(ID, *out_pParams);
		return;
	}

	*out_pParams = (*it->second->GetParams());
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation ** out_ppTriangulation) 
{
	std::lock_guard<std::mutex> objLock(_objectsMutex);

	auto itObj = _mapId2Object.find(ID);

	if (itObj == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		*out_ppTriangulation = nullptr;
		return;
	}

	if (!itObj->second->IsTriangulationReady())
	{
		*out_ppTriangulation = nullptr;
		return;
	}

	*out_ppTriangulation = itObj->second->GetTriangulation();
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8])
{
	_pTerrainObjectManager->GetTerrainObjectNeighbours(ID, outNeighbours);
}

size_t CTerrainManager::CTerrainManagerImpl::GetNewObjectsCount() const
{
	//std::lock_guard<std::mutex> lock(_containersMutex);
	
	return _vecNewObjectIDs.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetNewObjectID(size_t index) const
{
	//std::lock_guard<std::mutex> lock(_containersMutex);

	if (index >= _vecNewObjectIDs.size())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetNewObjectID: Illegal object id (%d), objects count = %d", index, _vecNewObjectIDs.size());
		return -1;
	}

	return _vecNewObjectIDs[index];
}

// �������� ��� �������� ��� ������� �������
const wchar_t* CTerrainManager::CTerrainManagerImpl::GetTextureFileName(TerrainObjectID ID) const
{
	std::lock_guard<std::mutex> lock(_objectsMutex);

	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	return it->second->GetTextureFileName();
}

// �������� ��� ����� ����� ��� ������� �������
const wchar_t* CTerrainManager::CTerrainManagerImpl::GetHeightmapFileName(TerrainObjectID ID) const
{
	std::lock_guard<std::mutex> lock(_objectsMutex);

	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	return it->second->GetHeightmapFileName();
}

size_t CTerrainManager::CTerrainManagerImpl::GetObjectsToDeleteCount() const
{
	return _vecObjectsToDelete.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetObjectToDeleteID(size_t index) const
{
	if (index >= _vecObjectsToDelete.size())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetObjectToDeleteID: Illegal object id (%d), objects count = %d", index, _vecObjectsToDelete.size());
		return -1;
	}

	return _vecObjectsToDelete[index];
}

size_t CTerrainManager::CTerrainManagerImpl::GetVisibleObjectsCount() const
{
	return _vecReadyVisibleObjects.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetVisibleObjectID(size_t index) const
{
	return _vecReadyVisibleObjects[index];
}

void CTerrainManager::CTerrainManagerImpl::SetTextureReady(TerrainObjectID ID)
{
	std::lock_guard<std::mutex> lock(_objectsMutex);
	auto it = _mapId2Object.find(ID);

	if (it != _mapId2Object.end())
	{
		it->second->SetTextureReady();
	}
}

void CTerrainManager::CTerrainManagerImpl::SetHeightmapReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap)
{
	std::lock_guard<std::mutex> hfLock(_objectHeightfieldsMutex);

	SObjectHeightfield& hf = _mapObjectHeightfields[ID];

	hf._ready = true;
	hf._heightfield.pTextureSRV = in_pLoadedHeightmap;

	if (in_pLoadedHeightmap)
		hf._heightfield.pTextureSRV->AddRef();
}

void CTerrainManager::CTerrainManagerImpl::SetAwaitVisibleForDataReady(bool in_bAwait)
{
	_bAwaitingVisibleForDataReady = in_bAwait;
}

bool CTerrainManager::CTerrainManagerImpl::IsTriangulationReady(TerrainObjectID ID) const
{
	std::lock_guard<std::mutex> lock(_objectTriangulationsMutex);

	return _mapObjectTriangulations.find(ID) != _mapObjectTriangulations.end();
}

float CTerrainManager::CTerrainManagerImpl::GetWorldRadius() const
{
	return _fWorldSize;
}

float CTerrainManager::CTerrainManagerImpl::GetMinCellSize() const
{
	return _fWorldScale * 100.f;
}

vm::Vector3df GetTransformedPoint(const vm::Vector3df& point, const vm::Vector3df& vPos, const vm::Vector3df& vX, const vm::Vector3df& vY, const vm::Vector3df& vZ)
{
	vm::Vector3df vDelta = point - vPos;
	return vm::Vector3df(vm::dot_prod(vDelta, vX), vm::dot_prod(vDelta, vY), vm::dot_prod(vDelta, vZ));
}

void UpdateBBoxSurfacePoint(vm::BoundBox<double>& out_BB, float longi, float latti, float minH, float maxH, const vm::Vector3df& vPos, const vm::Vector3df& vX, const vm::Vector3df& vY, const vm::Vector3df& vZ)
{
	vm::Vector3df vPoint = GetWGS84SurfacePoint(longi, latti);
	vm::Vector3df vNormal = GetWGS84SurfaceNormal(longi, latti);

	vm::Vector3df p1 = (vPoint + vNormal*minH);
	vm::Vector3df p2 = (vPoint + vNormal*maxH);

	// For OBB:
	out_BB.update(GetTransformedPoint(p1, vPos, vX, vY, vZ));
	out_BB.update(GetTransformedPoint(p2, vPos, vX, vY, vZ));

	// For AABB:
	//out_BB.update(p1);
	//out_BB.update(p2);
}

void CTerrainManager::CTerrainManagerImpl::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords, unsigned int nLod)
{
	double middleLattitude = (in_Coords.fMinLattitude + in_Coords.fMaxLattitude)*0.5;
	double middleLongitude = (in_Coords.fMinLongitude + in_Coords.fMaxLongitude)*0.5;

	const double Rmin = 6356752.3142;

	double anglesDist = (in_Coords.fMaxLattitude - in_Coords.fMinLattitude);

	double diam = Rmin * anglesDist;

	//_vecLODDiameter[nLod] = std::max<float>(_vecLODDiameter[nLod], (float)diam);

	vm::Vector3df vMiddlePoint = GetWGS84SurfacePoint(middleLongitude, middleLattitude);
	vm::Vector3df vNormal = GetWGS84SurfaceNormal(vMiddlePoint);
	vm::Vector3df vEast = vm::normalize(vm::cross(vNormal, vm::Vector3df(0, 1, 0)));
	vm::Vector3df vNorth = vm::normalize(vm::cross(vNormal, vEast));

	vm::BoundBox<double> vBoundBox(vm::Vector3f(0, 0, 0)); // -> for OBB
//	vm::BoundBox<double> vBoundBox(vMiddlePoint); // -> For AABB

	double dfMinHeight = -10000.0;
	double dfMaxHeight = 20000.0;

	/*if (_pHeightfieldConverter)
	{
		dfMinHeight *= _pHeightfieldConverter->GetHeightScale();
		dfMaxHeight *= _pHeightfieldConverter->GetHeightScale();
	}*/

	double dfMinLat = in_Coords.fMinLattitude;
	double dfMaxLat = in_Coords.fMaxLattitude;
	double dfMinLong = in_Coords.fMinLongitude;
	double dfMaxLong = in_Coords.fMaxLongitude;

	double dfDeltaLat = (dfMaxLat - dfMinLat) / 10;
	double dfDeltaLong = (dfMaxLong - dfMinLong) / 10;

	for (double dfLat = dfMinLat; dfLat <= dfMaxLat; dfLat += dfDeltaLat)
	{
		for (double dfLong = dfMinLong; dfLong <= dfMaxLong; dfLong += dfDeltaLong)
		{
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight, vMiddlePoint, vEast, vNormal, vNorth);
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight, vMiddlePoint, vEast, vNormal, vNorth);
		}
	}


	memcpy(out_TriangulationCoords.vPosition, &vMiddlePoint[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vXAxis, &vEast[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vYAxis, &vNormal[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vZAxis, &vNorth[0], 3 * sizeof(double));


	memcpy(out_TriangulationCoords.vBoundBoxMinimum, &vBoundBox._vMin[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vBoundBoxMaximum, &vBoundBox._vMax[0], 3 * sizeof(double));

	for (unsigned int i = 0; i < 3; i++)
	{
		out_TriangulationCoords.vPosition[i] *= _fWorldScale;
		out_TriangulationCoords.vBoundBoxMinimum[i] *= _fWorldScale;
		out_TriangulationCoords.vBoundBoxMaximum[i] *= _fWorldScale;
	}
}

//@{ ������ �������� ��� ������� ����� ��������� ����� �����

size_t CTerrainManager::CTerrainManagerImpl::GetNewHeightmapsCount() const
{
	return _vecHeightmapsToCreate.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetNewHeightmapObjectID(size_t index) const
{
	return _vecHeightmapsToCreate[index];
}

//@}


void CTerrainManager::CTerrainManagerImpl::CreateObject(TerrainObjectID ID)
{
	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params);

	SHeightfield::SCoordinates coords;

	coords.fMinLattitude = params.fMinLattitude;
	coords.fMaxLattitude = params.fMaxLattitude;
	coords.fMinLongitude = params.fMinLongitude;
	coords.fMaxLongitude = params.fMaxLongitude;

	STriangulationCoordsInfo coordsInfo;
	ComputeTriangulationCoords(coords, coordsInfo, params.uiDepth);

	CInternalTerrainObject* pObject = new CInternalTerrainObject(ID, params, coordsInfo, _pTerrainObjectManager->GetTextureFileName(ID), _pTerrainObjectManager->GetHeighmapFileName(ID));

	{
		std::lock_guard<std::mutex> objLock(_objectsMutex);
		_mapId2Object[ID] = pObject;
	}

	if (!_pHeightfieldConverter)
		pObject->SetTriangulationReady(nullptr);

	_vecNewObjectIDs.push_back(pObject->GetID());

	std::lock_guard<std::mutex> lock(_containersMutex);
	_setNotReadyTriangulations.insert(pObject->GetID());
}

void CTerrainManager::CTerrainManagerImpl::DestroyObject(TerrainObjectID ID)
{
	_vecPreliminaryObjectsToDelete.push_back(ID);
}

void CTerrainManager::CTerrainManagerImpl::DestroyObjects()
{
	std::lock_guard<std::mutex> objectsLock(_objectsMutex);

	for (auto it = _mapId2Object.begin(); it != _mapId2Object.end(); it++)
	{
		delete it->second;
	}

	_mapId2Object.clear();
}

void CTerrainManager::CTerrainManagerImpl::ReleaseTriangulationsAndHeightmaps()
{
	if (_pHeightfieldConverter)
	{
		static std::vector<SHeightfield*> vecObjHF;
		static std::vector<STriangulation*> vecObjT;

		vecObjHF.clear();
		vecObjT.clear();

		{
			std::lock_guard<std::mutex> hfLock(_objectHeightfieldsMutex);

			for (auto it = _mapObjectHeightfields.begin(); it != _mapObjectHeightfields.end(); it++)
				vecObjHF.push_back(&it->second._heightfield);
		}

		{
			std::lock_guard<std::mutex> triLock(_objectTriangulationsMutex);

			for (auto it = _mapObjectTriangulations.begin(); it != _mapObjectTriangulations.end(); it++)
				vecObjT.push_back(&it->second._triangulation);
		}
		

		//for (SHeightfield* pHF : vecObjHF)
			//_pHeightfieldConverter->ReleaseHeightfield(pHF);

		for (STriangulation* pT : vecObjT)
			_pHeightfieldConverter->ReleaseTriangulation(pT);
	}

	{
		std::lock_guard<std::mutex> triLock(_objectTriangulationsMutex);
		_mapObjectTriangulations.clear();
	}

	std::lock_guard<std::mutex> hfLock(_objectHeightfieldsMutex);
	_mapObjectHeightfields.clear();
}

//@{ ������� ��������� ������� ���������� �����

// ���������� ������� ���������� ��� NLods �����
void CTerrainManager::CTerrainManagerImpl::SetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	if (_pTerrainVisibility)
		_pTerrainVisibility->SetLodDistancesKM(aLodDistances, NLods);
}

// ������� ������� ���������� ��� NLods �����
void CTerrainManager::CTerrainManagerImpl::GetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	if (_pTerrainVisibility)
		_pTerrainVisibility->GetLodDistancesKM(aLodDistances, NLods);
}

// ���������� ������������� ������� ���������� ������ �� ������������� ���������� �������� �� �������
// (�����������: FOV ������, ���������� ������, ������ ������� �����, �������� ������� ��������������� ������ �����)
void CTerrainManager::CTerrainManagerImpl::CalculateLodDistances(float in_fMaxPixelsPerTexel, unsigned int in_uiScreenResolutionX, unsigned int in_uiScreenResolutionY)
{
	_cameraParams.uiScreenResolutionX = in_uiScreenResolutionX;
	_cameraParams.uiScreenResolutionY = in_uiScreenResolutionY;
	_fMaxPixelsPerTexel = in_fMaxPixelsPerTexel;

	_bRecalculateLodsDistances = true;
}

void CTerrainManager::CTerrainManagerImpl::SetLastLodDistanceOnSurface(double distance)
{
	_pTerrainVisibility->SetLastLODDistanceOnSurface(distance);
}

//@}


//@{ ������� ��������� ���������� ��� ��������

// ��������� ��������� � ����������� ���������� �����������
void CTerrainManager::CTerrainManagerImpl::FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams)
{
	*out_pGlobalShaderParams = _globalTerrainShaderParams;
}

bool CTerrainManager::CTerrainManagerImpl::FillTerrainBlockParams(TerrainObjectID ID, STerrainBlockParams& out_Params) const
{
	std::lock_guard<std::mutex> lock(_objectsMutex);

	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return false;
	}

	out_Params = *(it->second->GetParams());

	return true;
}

// ��������� ��������� � ����������� ��� ���������� �����
void CTerrainManager::CTerrainManagerImpl::FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams)
{
	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params);

	SHeightfield* pHeightfield = RequestObjectHeightfield(ID);

	if (!pHeightfield)
	{
		LogMessage("Error in FillTerrainBlockShaderParams: cannot find heightfield for object %d", ID);
		return;
	}

	TerrainObjectID neighbours[8];
	GetTerrainObjectNeighbours(ID, neighbours);

	const SHeightfield* neighbourHeightfields[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	for (int i = 0; i < 8; i++)
	{
		if (neighbours[i] != INVALID_TERRAIN_OBJECT_ID)
		{
			if ((_setPreliminaryVisibleObjectIDs.find(neighbours[i]) == _setPreliminaryVisibleObjectIDs.end()) && 
				(_pTerrainObjectManager->GetObjectDepth(neighbours[i]) != 0))
			{
				neighbours[i] = _pTerrainObjectManager->GetTerrainObjectParent(neighbours[i]);

				if (_setPreliminaryVisibleObjectIDs.find(neighbours[i]) == _setPreliminaryVisibleObjectIDs.end())
					neighbours[i] = INVALID_TERRAIN_OBJECT_ID;
			}

			if (neighbours[i] != INVALID_TERRAIN_OBJECT_ID)
				neighbourHeightfields[i] = RequestObjectHeightfield(neighbours[i]);
		}
	}

	out_pTerrainBlockShaderParams->pHeightfield = pHeightfield->pTextureSRV;

	STriangulation* objTri = nullptr;
	GetTerrainObjectTriangulation(ID, &objTri);

	out_pTerrainBlockShaderParams->pVertexBuffer = objTri->pVertexBuffer;

	out_pTerrainBlockShaderParams->fMinLattitude = params.fMinLattitude;
	out_pTerrainBlockShaderParams->fMaxLattitude = params.fMaxLattitude;
												   
	out_pTerrainBlockShaderParams->fMinLongitude = params.fMinLongitude;
	out_pTerrainBlockShaderParams->fMaxLongitude = params.fMaxLongitude;

	out_pTerrainBlockShaderParams->nCountX = pHeightfield->Config.nCountX;// / _heightfieldCompressionRatio;
	out_pTerrainBlockShaderParams->nCountY = pHeightfield->Config.nCountY;// / _heightfieldCompressionRatio;


	out_pTerrainBlockShaderParams->fLongitudeCoeff = params.fLongitude�utCoeff;
	out_pTerrainBlockShaderParams->fLattitudeCoeff = params.fLattitudeCutCoeff;

	out_pTerrainBlockShaderParams->fWorldScale = _fWorldScale;

	if (_pHeightfieldConverter)
		out_pTerrainBlockShaderParams->fHeightScale = _pHeightfieldConverter->GetHeightScale();

	if (neighbourHeightfields[0])
	{
		out_pTerrainBlockShaderParams->fNorthBlockLongCoeff = neighbourHeightfields[0]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fNorthBlockLatCoeff = neighbourHeightfields[0]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fNorthMinLat = neighbourHeightfields[0]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fNorthMaxLat = neighbourHeightfields[0]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fNorthMinLong = neighbourHeightfields[0]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fNorthMaxLong = neighbourHeightfields[0]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pNorthNeighbourHeightfield = neighbourHeightfields[0]->pTextureSRV;
	}


	if (neighbourHeightfields[1])
	{
		out_pTerrainBlockShaderParams->fNorthEastBlockLongCoeff = neighbourHeightfields[1]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fNorthEastBlockLatCoeff = neighbourHeightfields[1]->fLattitudeCutCoeff;


		out_pTerrainBlockShaderParams->fNorthEastMinLat = neighbourHeightfields[1]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fNorthEastMaxLat = neighbourHeightfields[1]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fNorthEastMinLong = neighbourHeightfields[1]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fNorthEastMaxLong = neighbourHeightfields[1]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pNorthEastNeighbourHeightfield = neighbourHeightfields[1]->pTextureSRV;
	}

	if (neighbourHeightfields[2])
	{
		out_pTerrainBlockShaderParams->fEastBlockLongCoeff = neighbourHeightfields[2]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fEastBlockLatCoeff = neighbourHeightfields[2]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fEastMinLat = neighbourHeightfields[2]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fEastMaxLat = neighbourHeightfields[2]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fEastMinLong = neighbourHeightfields[2]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fEastMaxLong = neighbourHeightfields[2]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pEastNeighbourHeightfield = neighbourHeightfields[2]->pTextureSRV;
	}

	if (neighbourHeightfields[3])
	{
		out_pTerrainBlockShaderParams->fSouthEastBlockLongCoeff = neighbourHeightfields[3]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fSouthEastBlockLatCoeff = neighbourHeightfields[3]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fSouthEastMinLat = neighbourHeightfields[3]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fSouthEastMaxLat = neighbourHeightfields[3]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fSouthEastMinLong = neighbourHeightfields[3]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fSouthEastMaxLong = neighbourHeightfields[3]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pSouthEastNeighbourHeightfield = neighbourHeightfields[3]->pTextureSRV;
	}

	if (neighbourHeightfields[4])
	{
		out_pTerrainBlockShaderParams->fSouthBlockLongCoeff = neighbourHeightfields[4]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fSouthBlockLatCoeff = neighbourHeightfields[4]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fSouthMinLat = neighbourHeightfields[4]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fSouthMaxLat = neighbourHeightfields[4]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fSouthMinLong = neighbourHeightfields[4]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fSouthMaxLong = neighbourHeightfields[4]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pSouthNeighbourHeightfield = neighbourHeightfields[4]->pTextureSRV;
	}

	if (neighbourHeightfields[5])
	{
		out_pTerrainBlockShaderParams->fSouthWestBlockLongCoeff = neighbourHeightfields[5]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fSouthWestBlockLatCoeff = neighbourHeightfields[5]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fSouthWestMinLat = neighbourHeightfields[5]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fSouthWestMaxLat = neighbourHeightfields[5]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fSouthWestMinLong = neighbourHeightfields[5]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fSouthWestMaxLong = neighbourHeightfields[5]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pSouthWestNeighbourHeightfield = neighbourHeightfields[5]->pTextureSRV;
	}

	if (neighbourHeightfields[6])
	{
		out_pTerrainBlockShaderParams->fWestBlockLongCoeff = neighbourHeightfields[6]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fWestBlockLatCoeff = neighbourHeightfields[6]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fWestMinLat = neighbourHeightfields[6]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fWestMaxLat = neighbourHeightfields[6]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fWestMinLong = neighbourHeightfields[6]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fWestMaxLong = neighbourHeightfields[6]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pWestNeighbourHeightfield = neighbourHeightfields[6]->pTextureSRV;
	}

	if (neighbourHeightfields[7])
	{
		out_pTerrainBlockShaderParams->fNorthWestBlockLongCoeff = neighbourHeightfields[7]->fLongitudeCutCoeff;
		out_pTerrainBlockShaderParams->fNorthWestBlockLatCoeff = neighbourHeightfields[7]->fLattitudeCutCoeff;

		out_pTerrainBlockShaderParams->fNorthWestMinLat = neighbourHeightfields[7]->Config.Coords.fMinLattitude;
		out_pTerrainBlockShaderParams->fNorthWestMaxLat = neighbourHeightfields[7]->Config.Coords.fMaxLattitude;

		out_pTerrainBlockShaderParams->fNorthWestMinLong = neighbourHeightfields[7]->Config.Coords.fMinLongitude;
		out_pTerrainBlockShaderParams->fNorthWestMaxLong = neighbourHeightfields[7]->Config.Coords.fMaxLongitude;

		out_pTerrainBlockShaderParams->pNorthWestNeighbourHeightfield = neighbourHeightfields[7]->pTextureSRV;
	}


	out_pTerrainBlockShaderParams->uiCurrentLOD = params.uiDepth;

	
	for (int i = 0; i < 4; i++)
		out_pTerrainBlockShaderParams->uiAdjacentLOD[i] =  0;



	if (neighbours[0] != INVALID_TERRAIN_OBJECT_ID)
		out_pTerrainBlockShaderParams->uiAdjacentLOD[0] = _pTerrainObjectManager->GetObjectDepth(neighbours[0]);
	else
		out_pTerrainBlockShaderParams->uiAdjacentLOD[0] = -1;

	if (neighbours[6] != INVALID_TERRAIN_OBJECT_ID)
		out_pTerrainBlockShaderParams->uiAdjacentLOD[1] = _pTerrainObjectManager->GetObjectDepth(neighbours[6]);
	else
		out_pTerrainBlockShaderParams->uiAdjacentLOD[1] = -1;

	if (neighbours[4] != INVALID_TERRAIN_OBJECT_ID)
		out_pTerrainBlockShaderParams->uiAdjacentLOD[2] = _pTerrainObjectManager->GetObjectDepth(neighbours[4]);
	else
		out_pTerrainBlockShaderParams->uiAdjacentLOD[2] = -1;

	if (neighbours[2] != INVALID_TERRAIN_OBJECT_ID)
		out_pTerrainBlockShaderParams->uiAdjacentLOD[3] = _pTerrainObjectManager->GetObjectDepth(neighbours[2]);
	else
		out_pTerrainBlockShaderParams->uiAdjacentLOD[3] = -1;
}

//@}