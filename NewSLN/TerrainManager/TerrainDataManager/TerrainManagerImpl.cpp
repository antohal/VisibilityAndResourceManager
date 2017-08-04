#include <Shlwapi.h>

#include "TerrainManagerImpl.h"
#include "Log.h"
#include "FileUtil.h"
#include "wgs84.h"

#include <d3dx10math.h>
#include <algorithm>

#include "TerrainVisibilityManagerImpl.h"

#define USE_ENGINE_SCALE

#ifdef USE_ENGINE_SCALE
const float g_fMasterScale = 100.f;
#else
const float g_fMasterScale = 1.f;
#endif

CInternalTerrainObject::CInternalTerrainObject(C3DBaseManager* in_pOwner, TerrainObjectID ID, 
	const CTerrainBlockDesc* in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo)
	: _pOwner(in_pOwner), _ID(ID), _pBlockDesc(in_pBlockDesc)
{
	_vBBoxMin = D3DXVECTOR3(
		(float)in_coordsInfo.vBoundBoxMinimum[0],
		(float)in_coordsInfo.vBoundBoxMinimum[1],
		(float)in_coordsInfo.vBoundBoxMinimum[2]
	);

	_vBBoxMax = D3DXVECTOR3(
		(float)in_coordsInfo.vBoundBoxMaximum[0],
		(float)in_coordsInfo.vBoundBoxMaximum[1],
		(float)in_coordsInfo.vBoundBoxMaximum[2]
	);

	D3DXMatrixIdentity(&_mTransform);
}

void CInternalTerrainObject::GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax)
{
	*ppBBMin = &_vBBoxMin;
	*ppBBMax = &_vBBoxMax;
}

D3DXMATRIX* CInternalTerrainObject::GetWorldTransform()
{
	return &_mTransform;
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
void CTerrainManager::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff)
{
	_implementation->Init(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, in_fWorldScale, in_fWorldSize, in_fLongitudeScaleCoeff, in_fLattitudeScaleCoeff);
}

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

void CTerrainManager::InitGenerated(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize)
{
	_implementation->InitGenerated(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, N, M, depth, in_fWorldScale, in_fWorldSize);
}

void CTerrainManager::SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection)
{
	_implementation->SetViewProjection(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

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
const STerrainBlockParams*	CTerrainManager::GetTerrainObjectParams(TerrainObjectID ID) const
{
	return _implementation->GetTerrainObjectParams(ID);
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

void CTerrainManager::SetDataReady(TerrainObjectID ID)
{
	_implementation->SetDataReady(ID);
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

//@}


// �������� ��������� �� �������� �������� (���� ���������� ������ ���������� ������������� ��������� ��������, �������� �� �������� ��-���������)
CResourceManager* CTerrainManager::GetResourceManager()
{
	return _implementation->GetResourceManager();
}

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
	_vecLODDiameter[0] = 2 * Rmin;
}

CTerrainManager::CTerrainManagerImpl::~CTerrainManagerImpl()
{
	if (_pResourceManager)
		delete _pResourceManager;

	if (_pTerrainVisibilityManager)
		delete _pTerrainVisibilityManager;

	if (_pVisibilityManager)
		delete _pVisibilityManager;

	DestroyObjects();
	ReleaseTriangulationsAndHeightmaps();

	if (_pPlanetTerrainData)
		_pTerrainDataManager->ReleaseTerrainDataInfo(_pPlanetTerrainData);

	if (_pTerrainDataManager)
		delete _pTerrainDataManager;
}

void CTerrainManager::CTerrainManagerImpl::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t * in_pcwszPlanetDirectory, float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff)
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pResourceManager = new CResourceManager();

	_fWorldSize = in_fWorldSize;
	_fWorldScale = g_fMasterScale*in_fWorldScale;

	LogMessage("Loading planet terrain info");

	unsigned int uiMaxDepth = 0;
	_pTerrainDataManager->LoadTerrainDataInfo(std::wstring(GetStartDir() + in_pcwszPlanetDirectory).c_str(), &_pPlanetTerrainData, in_fLongitudeScaleCoeff, in_fLattitudeScaleCoeff, &uiMaxDepth);

	CreateObjects();

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());
	_pResourceManager->AddVisibilityManager(_pVisibilityManager);


	_pTerrainVisibilityManager = new CTerrainVisibilityManager;
	_pTerrainVisibilityManager->Init(this, _fWorldScale, 6000000.0f, 0.5, uiMaxDepth);

	_pVisibilityManager->InstallPlugin(_pTerrainVisibilityManager);
}

void CTerrainManager::CTerrainManagerImpl::InitFromDatabaseInfo(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency)
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pResourceManager = new CResourceManager();

	_fWorldSize = in_fWorldSize;
	_fWorldScale = g_fMasterScale*in_fWorldScale;

	LogMessage("Loading planet terrain info");

	// Read data 

	DataBaseInfo dbInfo;
	LodInfoStruct* aLods = nullptr;

	//std::wstring wsDbFileName = std::wstring(GetStartDir() + in_pcwszFileName);

	std::wstring wsDbFileName = in_pcwszFileName;

	if (!PathFileExistsW(wsDbFileName.c_str()))
		wsDbFileName = std::wstring(GetStartDir() + in_pcwszFileName);

	if (!PathFileExistsW(wsDbFileName.c_str()))
	{
		LogMessage("Cannot open database file %ls", wsDbFileName.c_str());
		return;
	}

	bool bSuccessifulRead = true;

	FILE* fp = nullptr;
	
	_wfopen_s(&fp, wsDbFileName.c_str(), L"rb");

	if (fp)
	{
		if (fread_s(&dbInfo, sizeof(DataBaseInfo), sizeof(DataBaseInfo), 1, fp) != 1)
		{
			bSuccessifulRead = false;
		}

		if (bSuccessifulRead)
		{
			aLods = new LodInfoStruct[dbInfo.LodCount];

			size_t nReadLods = fread_s(aLods, sizeof(LodInfoStruct) * dbInfo.LodCount, sizeof(LodInfoStruct), dbInfo.LodCount, fp);

			if (nReadLods != dbInfo.LodCount)
			{
				LogMessage("Error reading lod elements from file %ls, aborting. Readed only %d, while expected %d", in_pcwszFileName, nReadLods, dbInfo.LodCount);
				bSuccessifulRead = false;
			}
		}

		fclose(fp);
	}
	else
	{
		LogMessage("Cannot open database file %ls, aborting", in_pcwszFileName);
		return;
	}

	if (!bSuccessifulRead)
	{
		LogMessage("Error reading earth database file %ls, aborting", in_pcwszFileName);
		return;
	}
	else
	{

		for (int i = 0; i < dbInfo.LodCount; i++)
		{
			_vecLODResolution[i] = std::max<short>(aLods[i].Width, aLods[i].Height);
		}

	}

	unsigned int uiMaxDepth = std::min<unsigned int>(dbInfo.LodCount, in_uiMaxDepth);

	// TODO: Read lods structure

	_wsPlanetRootDirectory = ExtractFileDirectory(wsDbFileName);
	
	unsigned int uiResultingMaxDepth = 0;
	_pTerrainDataManager->LoadTerrainDataInfo(_wsPlanetRootDirectory.c_str(), dbInfo, aLods, uiMaxDepth, &_pPlanetTerrainData, &uiResultingMaxDepth, in_bCalculateAdjacency);

	CreateObjects();

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());
	_pResourceManager->AddVisibilityManager(_pVisibilityManager);


	_pTerrainVisibilityManager = new CTerrainVisibilityManager;
	_pTerrainVisibilityManager->Init(this, _fWorldScale, 6000000.0f, 0.5, uiResultingMaxDepth);

	_pVisibilityManager->InstallPlugin(_pTerrainVisibilityManager);

	delete[] aLods;
}

void CTerrainManager::CTerrainManagerImpl::InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize)
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pResourceManager = new CResourceManager();

	_fWorldScale = g_fMasterScale*in_fWorldScale;
	_fWorldSize = in_fWorldSize;

	LogMessage("Generating planet terrain info");

	_pTerrainDataManager->GenerateTerrainDataInfo(in_pcwszPlanetDirectory, &_pPlanetTerrainData, M, N, depth);

	CreateObjects();

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());
	_pResourceManager->AddVisibilityManager(_pVisibilityManager);

	_pTerrainVisibilityManager = new CTerrainVisibilityManager;
	_pTerrainVisibilityManager->Init(this, _fWorldScale, 6000000.0f, 0.5f, depth);

	_pVisibilityManager->InstallPlugin(_pTerrainVisibilityManager);
}

void CTerrainManager::CTerrainManagerImpl::SetHeightfieldConverter(HeightfieldConverter * in_pHeightfieldConverter)
{
	_pHeightfieldConverter = in_pHeightfieldConverter;
}

void CTerrainManager::CTerrainManagerImpl::SetViewProjection(const D3DXVECTOR3 * in_vPos, const D3DXVECTOR3 * in_vDir, const D3DXVECTOR3 * in_vUp, const D3DMATRIX * in_pmProjection)
{
	Vector3 vPos, vDir, vUp;

	vPos.x = (float)in_vPos->x;
	vPos.y = (float)in_vPos->y;
	vPos.z = (float)in_vPos->z;

	vDir.x = (float)in_vDir->x;
	vDir.y = (float)in_vDir->y;
	vDir.z = (float)in_vDir->z;

	vUp.x = (float)in_vUp->x;
	vUp.y = (float)in_vUp->y;
	vUp.z = (float)in_vUp->z;


	if (in_pmProjection)
		_cameraParams.mProjection = *in_pmProjection;

	_cameraParams.vPos = vm::Vector3df(vPos.x / _fWorldScale, vPos.y / _fWorldScale, vPos.z / _fWorldScale);
	_cameraParams.vDir = vm::Vector3df(vDir.x, vDir.y, vDir.z);
	_cameraParams.vUp = vm::Vector3df(vUp.x, vUp.y, vUp.z);

	_pVisibilityManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
	_pResourceManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));

	_pVisibilityManager->GetFOVAnglesDeg(_cameraParams.fHFovAngleRad, _cameraParams.fVFovAngleRad);

	_cameraParams.fHFovAngleRad *= D2R;
	_cameraParams.fVFovAngleRad *= D2R;
}

SHeightfield*	CTerrainManager::CTerrainManagerImpl::RequestObjectHeightfield(TerrainObjectID ID)
{
	SHeightfield*	pHeightfield = nullptr;

	_triangulationsMutex.lock();

	auto it = _mapObjectHeightfields.find(ID);
	if (it != _mapObjectHeightfields.end())
	{
		it->second._timeSinceLastRequest = 0;

		pHeightfield = &it->second._heightfield;

		_triangulationsMutex.unlock();

		return pHeightfield;
	}
	else
	{
		SObjectHeightfield& objHF = _mapObjectHeightfields[ID];

		pHeightfield = &objHF._heightfield;
	}

	_triangulationsMutex.unlock();


	std::wstring wsHeightmapFileName = GetHeightmapFileName(ID);
	const STerrainBlockParams* pParams = GetTerrainObjectParams(ID);

	// ������� ������ ����� ����� �� �����
	_pHeightfieldConverter->ReadHeightfieldDataFromTexture(wsHeightmapFileName.c_str(), *pHeightfield, (unsigned short)_heightfieldCompressionRatio);

	LogMessage("Loading faceset. Triangulating heightmap '%ls'", wsHeightmapFileName.c_str());

	// �������� ��������� ������
	pHeightfield->Config.Coords.fMinLattitude = pParams->fMinLattitude;
	pHeightfield->Config.Coords.fMaxLattitude = pParams->fMaxLattitude;
	pHeightfield->Config.Coords.fMinLongitude = pParams->fMinLongitude;
	pHeightfield->Config.Coords.fMaxLongitude = pParams->fMaxLongitude;

	pHeightfield->fLattitudeCutCoeff = pParams->fLattitudeCutCoeff;
	pHeightfield->fLongitudeCutCoeff = pParams->fLongitude�utCoeff;

	return pHeightfield;
}

void CTerrainManager::CTerrainManagerImpl::Update(float in_fDeltaTime)
{

	if (_bRecalculateLodsDistances)
	{
		_pTerrainVisibilityManager->_implementation->CalculateLodDistances(0.5f*(_cameraParams.fHFovAngleRad + _cameraParams.fVFovAngleRad), _vecLODResolution, _vecLODDiameter,
			(_cameraParams.uiScreenResolutionX + _cameraParams.uiScreenResolutionY) / 2, _fMaxPixelsPerTexel);
	}

	_containersMutex.lock();

	_vecNewObjectIDs.clear();
	_vecObjectsToDelete.clear();

	_containersMutex.unlock();

	_pVisibilityManager->UpdateVisibleObjectsSet();
	_pResourceManager->Update(in_fDeltaTime);

	if (_pHeightfieldConverter)
	{
		std::lock_guard<std::mutex> lock(_triangulationsMutex);

		for (auto it = _mapObjectHeightfields.begin(); it != _mapObjectHeightfields.end();)
		{
			if (it->second._timeSinceLastRequest > 10.0)
			{
				_pHeightfieldConverter->ReleaseHeightfield(&it->second._heightfield);

				it = _mapObjectHeightfields.erase(it);
			}
			else
			{
				it->second._timeSinceLastRequest += in_fDeltaTime;
				it++;
			}
		}

		for (auto it = _mapObjectTriangulations.begin(); it != _mapObjectTriangulations.end();)
		{
			if (it->second._alive)
			{
				it++;
				continue;
			}

			if (it->second._timeSinceDead > 10.f)
			{
				_pHeightfieldConverter->ReleaseTriangulation(&it->second._triangulation);

				it = _mapObjectTriangulations.erase(it);
			}
			else
			{
				if (!it->second._alive)
					it->second._timeSinceDead += in_fDeltaTime;

				it++;
			}
		}
	}

	static std::set<CInternalTerrainObject*> setVisObjs;
	setVisObjs.clear();

	bool bAllReady = true;

	for (size_t iVisObj = 0; iVisObj < _pVisibilityManager->GetVisibleObjectsCount(); iVisObj++)
	{
		CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(_pVisibilityManager->GetVisibleObjectPtr(iVisObj));

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
		//_vecReadyVisibleObjects = vecVisID;

		_containersMutex.lock();

		_vecObjectsToDelete = _vecPreliminaryObjectsToDelete;
		_vecPreliminaryObjectsToDelete.clear();

		for (TerrainObjectID deadObj : _vecObjectsToDelete)
		{
			std::lock_guard<std::mutex> lock(_triangulationsMutex);

			auto itTri = _mapObjectTriangulations.find(deadObj);
			if (itTri == _mapObjectTriangulations.end())
			{
				LogMessage("Object %d has no triangulation while was alive", deadObj);
				continue;
			}

			itTri->second._alive = false;
		}

		_containersMutex.unlock();
	}

	CalculateReadyAndVisibleSet();
}

void CInternalTerrainObject::CalculateReferencePoints(std::vector<vm::Vector3df>& out_vecPoints, std::vector<vm::Vector3df>& out_vecNormals)
{
	out_vecPoints.clear();
	out_vecNormals.clear();

	const STerrainBlockParams* pParams = _pBlockDesc->GetParams();
	double dfMinLat = pParams->fMinLattitude;
	double dfMaxLat = pParams->fMaxLattitude;
	double dfMinLong = pParams->fMinLongitude;
	double dfMaxLong = pParams->fMaxLongitude;

	double dfDeltaLat = (dfMaxLat - dfMinLat) / 10;
	double dfDeltaLong = (dfMaxLong - dfMinLong) / 10;

	for (double dfLat = dfMinLat; dfLat <= dfMaxLat; dfLat += dfDeltaLat)
	{
		for (double dfLong = dfMinLong; dfLong <= dfMaxLong; dfLong += dfDeltaLong)
		{
			vm::Vector3df vPoint = GetWGS84SurfacePoint(dfLong, dfLat);
			vm::Vector3df vNormal = GetWGS84SurfaceNormal(dfLong, dfLat);

			out_vecPoints.push_back(vPoint);
			out_vecNormals.push_back(vNormal);
		}
	}


}

bool CTerrainManager::CTerrainManagerImpl::CheckPointsInFrustum(const std::vector<vm::Vector3df>& vecPoints) const
{
	bool bAllPointsOutOfFrustum = true;

	for (const vm::Vector3df& vPoint : vecPoints)
	{
		if (dot(vPoint - _cameraParams.vPos, _cameraParams.vDir) >= 0)
		{
			bAllPointsOutOfFrustum = false;
			break;
		}
		// TODO: other checks
	}

	return !bAllPointsOutOfFrustum;
}

void CTerrainManager::CTerrainManagerImpl::CalculateReadyAndVisibleSet()
{
	_vecReadyVisibleObjects.clear();

	std::vector<vm::Vector3df> vecRefPoints;
	std::vector<vm::Vector3df> vecRefNormals;

	for (CInternalTerrainObject* pObj : _setPreliminaryVisibleObjects)
	{
		pObj->CalculateReferencePoints(vecRefPoints, vecRefNormals);

		if (!CheckPointsInFrustum(vecRefPoints))
			continue;


		// check backfaces
		bool bFullBackSided = true;

		for (size_t i = 0; i < vecRefNormals.size(); i++)
		{
			if (dot(normalize(vecRefPoints[i] - _cameraParams.vPos), normalize(vecRefPoints[i])) < 0.2)
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
	std::lock_guard<std::mutex> lock(_triangulationsMutex);
	return _mapObjectTriangulations.size();
}

size_t CTerrainManager::CTerrainManagerImpl::GetHeightfieldsCount() const
{
	std::lock_guard<std::mutex> lock(_triangulationsMutex);
	return _mapObjectHeightfields.size();
}


bool CTerrainManager::CTerrainManagerImpl::UpdateTriangulations()
{
	if (!_pHeightfieldConverter)
		return false;

	if (_vecNotCheckedForTriangulations.empty())
		return false;

	static std::vector<std::pair<TerrainObjectID, SObjectTriangulation*>> s_vecTriangulationsToCreate;
	s_vecTriangulationsToCreate.clear();

	_containersMutex.lock();
	_triangulationsMutex.lock();

	for (TerrainObjectID ID : _vecNotCheckedForTriangulations)
	{
		CInternalTerrainObject* pInternalObject = _mapId2Object[ID];

		if (!pInternalObject)
		{
			LogMessage("Unknown object with id: %d", ID);
			continue;
		}

		auto itExisting = _mapObjectTriangulations.find(ID);
		if (itExisting != _mapObjectTriangulations.end())
		{
			itExisting->second._alive = true;
			itExisting->second._timeSinceDead = 0;

			pInternalObject->SetTriangulationReady();

			continue;
		}

		SObjectTriangulation& objTri = _mapObjectTriangulations[ID];

		objTri._alive = true;
		objTri._timeSinceDead = 0;

		s_vecTriangulationsToCreate.push_back(std::make_pair(ID, &objTri));
	}

	_vecNotCheckedForTriangulations.clear();

	_containersMutex.unlock();
	_triangulationsMutex.unlock();


	for (std::pair<TerrainObjectID, SObjectTriangulation*>& pt : s_vecTriangulationsToCreate)
	{
		TerrainObjectID ID = pt.first;
		SObjectTriangulation& objTri = *(pt.second);

		CInternalTerrainObject* pInternalObject = _mapId2Object[ID];

		const STerrainBlockParams* pParams = GetTerrainObjectParams(ID);

		SHeightfield* pHeightfield = RequestObjectHeightfield(ID);

		TerrainObjectID neighbours[8];
		GetTerrainObjectNeighbours(ID, neighbours);

		const SHeightfield* neighbourHeightfields[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

		for (int i = 0; i < 8; i++)
		{

			if (neighbours[i] != (TerrainObjectID)(-1))
			{
				neighbourHeightfields[i] = RequestObjectHeightfield(neighbours[i]);
			}

		}

		// �������� ������������ � ������� ComputeShader. � ������� _triangulation ����� ��������� � ���������� ������
		_pHeightfieldConverter->CreateTriangulationImmediate(pHeightfield, pParams->fLongitude�utCoeff, pParams->fLattitudeCutCoeff, &objTri._triangulation, neighbourHeightfields);

		pInternalObject->SetTriangulationReady();
	}

	return true;
}

const STerrainBlockParams * CTerrainManager::CTerrainManagerImpl::GetTerrainObjectParams(TerrainObjectID ID) const
{
	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	return it->second->GetDesc()->GetParams();
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation ** out_ppTriangulation) 
{
	std::lock_guard<std::mutex> lock(_triangulationsMutex);

	auto it = _mapObjectTriangulations.find(ID);

	if (it == _mapObjectTriangulations.end())
	{
		*out_ppTriangulation = nullptr;
		return;
	}

	it->second._timeSinceDead = 0;
	*out_ppTriangulation = &(it->second._triangulation);
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8])
{
	const CTerrainBlockDesc* desc = GetTerrainDataForObject(_mapId2Object[ID]);
	if (!desc)
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectNeighbours: Illegal object id (%d)", ID);
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		if (_mapDesc2ID.find(desc->GetNeighbour(i)) != _mapDesc2ID.end())
			outNeighbours[i] = _mapDesc2ID[desc->GetNeighbour(i)];
		else
			outNeighbours[i] = -1;
	}
}

size_t CTerrainManager::CTerrainManagerImpl::GetNewObjectsCount() const
{
	std::lock_guard<std::mutex> lock(_containersMutex);
	
	return _vecNewObjectIDs.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetNewObjectID(size_t index) const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

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
	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	//static std::wstring wsTexFileName;
	//wsTexFileName = _wsPlanetRootDirectory + it->second->GetDesc()->GetTextureFileName();
	//return wsTexFileName.c_str();

	return it->second->GetDesc()->GetTextureFileName();
}

// �������� ��� ����� ����� ��� ������� �������
const wchar_t* CTerrainManager::CTerrainManagerImpl::GetHeightmapFileName(TerrainObjectID ID) const
{
	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	/*static std::wstring wsTexFileName;
	wsTexFileName = _wsPlanetRootDirectory + it->second->GetDesc()->GetHeightmapFileName();

	return wsTexFileName.c_str();*/

	return it->second->GetDesc()->GetHeightmapFileName();
}

size_t CTerrainManager::CTerrainManagerImpl::GetObjectsToDeleteCount() const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

	return _vecObjectsToDelete.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetObjectToDeleteID(size_t index) const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

	if (index >= _vecObjectsToDelete.size())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetObjectToDeleteID: Illegal object id (%d), objects count = %d", index, _vecObjectsToDelete.size());
		return -1;
	}

	return _vecObjectsToDelete[index];
}

size_t CTerrainManager::CTerrainManagerImpl::GetVisibleObjectsCount() const
{
	//return _pVisibilityManager->GetVisibleObjectsCount();

	return _vecReadyVisibleObjects.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetVisibleObjectID(size_t index) const
{
	/*CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(_pVisibilityManager->GetVisibleObjectPtr(index));

	if (!pTerrainObject)
		return -1;

	return pTerrainObject->GetID();*/


	return _vecReadyVisibleObjects[index];
}

CResourceManager * CTerrainManager::CTerrainManagerImpl::GetResourceManager()
{
	return _pResourceManager;
}


size_t CTerrainManager::CTerrainManagerImpl::GetObjectsCount() const
{
	return _vecObjects.size();
}

C3DBaseObject*	CTerrainManager::CTerrainManagerImpl::GetObjectByIndex(size_t id) const
{
	if (id >= _vecObjects.size())
	{
		LogMessage("CTerrainManager::CTerrainManagerImpl::GetObjectByIndex: Illegal object id (%d), objects count = %d", id, _vecObjects.size());
		return nullptr;
	}

	return _vecObjects[id];
}

const CTerrainBlockDesc * CTerrainManager::CTerrainManagerImpl::GetTerrainDataForObject(C3DBaseObject * pObject) const
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(pObject);

	return pTerrainObject->GetDesc();
}

const CTerrainBlockDesc * CTerrainManager::CTerrainManagerImpl::GetRootTerrainData() const
{
	return _pPlanetTerrainData;
}

void CTerrainManager::CTerrainManagerImpl::RequestLoadResource(C3DBaseResource* in_pResource)
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(in_pResource);

	if (!_pHeightfieldConverter)
		pTerrainObject->SetTriangulationReady();

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecNewObjectIDs.push_back(pTerrainObject->GetID());
	_vecNotCheckedForTriangulations.push_back(pTerrainObject->GetID());
}

void CTerrainManager::CTerrainManagerImpl::RequestUnloadResource(C3DBaseResource* in_pResource)
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(in_pResource);

	pTerrainObject->InvalidateData();

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecPreliminaryObjectsToDelete.push_back(pTerrainObject->GetID());
}


void CTerrainManager::CTerrainManagerImpl::SetDataReady(TerrainObjectID ID)
{
	auto it = _mapId2Object.find(ID);

	if (it != _mapId2Object.end())
	{
		it->second->SetDataReady();
	}
}

void CTerrainManager::CTerrainManagerImpl::SetAwaitVisibleForDataReady(bool in_bAwait)
{
	_bAwaitingVisibleForDataReady = in_bAwait;
}

bool CTerrainManager::CTerrainManagerImpl::IsTriangulationReady(TerrainObjectID ID) const
{
	std::lock_guard<std::mutex> lock(_triangulationsMutex);

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

void CTerrainManager::CTerrainManagerImpl::CreateObjects()
{
	if (!_pPlanetTerrainData)
	{
		LogMessage("Terrain data not loaded. Error.");
		return;
	}

	LogMessage("Terrain data loaded. Creating objects.");

	CreateObjectsRecursive(_pPlanetTerrainData);

	LogMessage("%d Objects created.", _vecObjects.size());
}

void CTerrainManager::CTerrainManagerImpl::CreateObjectsRecursive(const CTerrainBlockDesc* in_pData)
{
	if (in_pData->GetParentBlockDesc())
	{
		CreateObject(in_pData);
	}

	for (unsigned int i = 0; i < in_pData->GetChildBlockDescCount(); i++)
	{
		CreateObjectsRecursive(in_pData->GetChildBlockDesc(i));
	}
}


void UpdateBBoxSurfacePoint(vm::BoundBox<double>& out_BB, float longi, float latti, float minH, float maxH)
{
	vm::Vector3df vPoint = GetWGS84SurfacePoint(longi, latti);
	vm::Vector3df vNormal = GetWGS84SurfaceNormal(longi, latti);

	out_BB.update(vPoint + vNormal*minH);
	out_BB.update(vPoint + vNormal*maxH);
}


void CTerrainManager::CTerrainManagerImpl::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords, unsigned int nLod)
{
	double middleLattitude = (in_Coords.fMinLattitude + in_Coords.fMaxLattitude)*0.5;
	double middleLongitude = (in_Coords.fMinLongitude + in_Coords.fMaxLongitude)*0.5;

	const double Rmin = 6356752.3142;
	double diam = Rmin * ((in_Coords.fMaxLongitude - in_Coords.fMinLongitude) + (in_Coords.fMaxLattitude - in_Coords.fMinLattitude))*0.5;

	_vecLODDiameter[nLod - 1] = std::max<float>(_vecLODDiameter[nLod], (float)diam);

	vm::Vector3df vMiddlePoint = GetWGS84SurfacePoint(middleLongitude, middleLattitude);
	vm::Vector3df vNormal = GetWGS84SurfaceNormal(vMiddlePoint);
	vm::Vector3df vEast = vm::normalize(vm::cross(vNormal, vm::Vector3df(0, 1, 0)));
	vm::Vector3df vNorth = vm::normalize(vm::cross(vNormal, vEast));

	memcpy(out_TriangulationCoords.vPosition, &vMiddlePoint[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vXAxis, &vNorth[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vYAxis, &vNormal[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vZAxis, &vEast[0], 3 * sizeof(double));

	vm::BoundBox<double> vBoundBox(vMiddlePoint);

	const double dfMinHeight = -5000.0;
	const double dfMaxHeight = 10000.0;

	vBoundBox.update(vMiddlePoint + vNormal*dfMinHeight);
	vBoundBox.update(vMiddlePoint + vNormal*dfMaxHeight);

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
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight);
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight);
		}
	}


	memcpy(out_TriangulationCoords.vBoundBoxMinimum, &vBoundBox._vMin[0], 3 * sizeof(double));
	memcpy(out_TriangulationCoords.vBoundBoxMaximum, &vBoundBox._vMax[0], 3 * sizeof(double));


	for (unsigned int i = 0; i < 3; i++)
	{
		out_TriangulationCoords.vPosition[i] *= _fWorldScale;
		out_TriangulationCoords.vBoundBoxMinimum[i] *= _fWorldScale;
		out_TriangulationCoords.vBoundBoxMaximum[i] *= _fWorldScale;
	}
}

void CTerrainManager::CTerrainManagerImpl::CreateObject(const CTerrainBlockDesc* in_pData)
{
	SHeightfield::SCoordinates coords;

	coords.fMinLattitude = in_pData->GetParams()->fMinLattitude;
	coords.fMaxLattitude = in_pData->GetParams()->fMaxLattitude;
	coords.fMinLongitude = in_pData->GetParams()->fMinLongitude;
	coords.fMaxLongitude = in_pData->GetParams()->fMaxLongitude;

	STriangulationCoordsInfo coordsInfo;
	ComputeTriangulationCoords(coords, coordsInfo, in_pData->GetParams()->uiDepth);

	CInternalTerrainObject* pObject = new CInternalTerrainObject(static_cast<C3DBaseManager*>(this), _idCurrentIDForNewObject, in_pData, coordsInfo);

	_vecObjects.push_back(pObject);
	_mapId2Object[_idCurrentIDForNewObject] = pObject;
	_mapDesc2ID[in_pData] = _idCurrentIDForNewObject;

	_idCurrentIDForNewObject++;
}

void CTerrainManager::CTerrainManagerImpl::DestroyObjects()
{
	for (CInternalTerrainObject* pObject : _vecObjects)
	{
		delete pObject;
	}

	_vecObjects.clear();
}

void CTerrainManager::CTerrainManagerImpl::ReleaseTriangulationsAndHeightmaps()
{

	if (_pHeightfieldConverter)
	{
		static std::vector<SHeightfield*> vecObjHF;
		static std::vector<STriangulation*> vecObjT;

		vecObjHF.clear();
		vecObjT.clear();

		_triangulationsMutex.lock();

		for (auto it = _mapObjectHeightfields.begin(); it != _mapObjectHeightfields.end(); it++)
			vecObjHF.push_back(&it->second._heightfield);

		for (auto it = _mapObjectTriangulations.begin(); it != _mapObjectTriangulations.end(); it++)
			vecObjT.push_back(&it->second._triangulation);

		_triangulationsMutex.unlock();

		for (SHeightfield* pHF : vecObjHF)
			_pHeightfieldConverter->ReleaseHeightfield(pHF);

		for (STriangulation* pT : vecObjT)
			_pHeightfieldConverter->ReleaseTriangulation(pT);
	}

	_triangulationsMutex.lock();
	_mapObjectTriangulations.clear();
	_mapObjectHeightfields.clear();
	_triangulationsMutex.unlock();
}

//@{ ������� ��������� ������� ���������� �����

// ���������� ������� ���������� ��� NLods �����
void CTerrainManager::CTerrainManagerImpl::SetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	_pTerrainVisibilityManager->_implementation->SetLodDistancesKM(aLodDistances, NLods);
}

// ������� ������� ���������� ��� NLods �����
void CTerrainManager::CTerrainManagerImpl::GetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	_pTerrainVisibilityManager->_implementation->GetLodDistancesKM(aLodDistances, NLods);
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

//@}