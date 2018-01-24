#include <Shlwapi.h>

#include "TerrainManagerImpl.h"
#include "Log.h"
#include "FileUtil.h"
#include "wgs84.h"

#include <d3dx10math.h>
#include <algorithm>

//#include "Geometry/InFrustum.h"
#include "Geometry/Vector3D.h"
#include "Geometry/Matrix.h"
#include "StlUtil.h"


#define MAX_TRIANGULATIONS_PER_FRAME 5

#define USE_ENGINE_SCALE

#ifdef USE_ENGINE_SCALE
const float g_fMasterScale = 100.f;
#else
const float g_fMasterScale = 1.f;
#endif

const float g_fMaxHFAliveTime = 2.f;


Vector3 ToVisManVec3(const vm::Vector3df& v)
{
	return Vector3((float)v[0], (float)v[1], (float)v[2]);
}

static Vector3f ToVec3(const Vector3& v)
{
	return Vector3f(v.x, v.y, v.z);
}

static Vector3 FromVec3(const Vector3f& v)
{
	return Vector3(v.x, v.y, v.z);
}


D3DXVECTOR3 ToD3DXVec3(const vm::Vector3df& v)
{
	return D3DXVECTOR3(v[0], v[1], v[2]);
}

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
// инициализация. Параметр - имя дериктории, где лежат данные Земли
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

//@{ Функции получения параметров для шейдеров

// заполнить структуру с глобальными шейдерными параметрами
void CTerrainManager::FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams)
{
	_implementation->FillGlobalShaderParams(out_pGlobalShaderParams);
}

// заполнить структуру с параметрами для указанного блока
void CTerrainManager::FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams)
{
	_implementation->FillTerrainBlockShaderParams(ID, out_pTerrainBlockShaderParams);
}

//@}

// В момент вызова этой функции формируется 4 списка: 
// объекты, которые нужно создать
// объекты, которые стали видимыми
// объекты, которые стали невидимыми
// объекты, которые нужно удалить
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

// получить имя текстуры для данного объекта
const wchar_t*	CTerrainManager::GetTextureFileName(TerrainObjectID ID) const
{
	return _implementation->GetTextureFileName(ID);
}

// получить имя карты высот для данного объекта
const wchar_t*	CTerrainManager::GetHeightmapFileName(TerrainObjectID ID) const
{
	return _implementation->GetHeightmapFileName(ID);
}

bool CTerrainManager::IsObjectDataReady(TerrainObjectID ID) const
{
	return _implementation->IsObjectDataReady(ID);
}


void CTerrainManager::SetBorderNormals(bool enable)
{
	_implementation->SetBorderNormals(enable);
}

//Получить описание объекта Земли по идентификатору
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

TerrainObjectID CTerrainManager::GetTerrainObjectParent(TerrainObjectID ID) const
{
	return _implementation->GetTerrainObjectParent(ID);
}

size_t CTerrainManager::GetTriangulationsCount() const
{
	return _implementation->GetTriangulationsCount();
}

size_t CTerrainManager::GetHeightfieldsCount() const
{
	return _implementation->GetHeightfieldsCount();
}

size_t CTerrainManager::GetPotentiallyVisibleObjectsCount() const
{
	return _implementation->GetPotentiallyVisibleObjectsCount();
}

//@{ Список новых объектов, которые нужно создать (могут стать видимыми)
size_t CTerrainManager::GetNewObjectsCount() const
{
	return _implementation->GetNewObjectsCount();
}

TerrainObjectID	CTerrainManager::GetNewObjectID(size_t index) const
{
	return _implementation->GetNewObjectID(index);
}
//@}


//@{ Список объектов, которые нужно удалить (выпали из списка потенциально видимых)
size_t CTerrainManager::GetObjectsToDeleteCount() const
{
	return _implementation->GetObjectsToDeleteCount();
}

TerrainObjectID CTerrainManager::GetObjectToDeleteID(size_t index) const
{
	return _implementation->GetObjectToDeleteID(index);
}

//@}

//@{ Список текущих видимых объектов
size_t CTerrainManager::GetVisibleObjectsCount() const
{
	return _implementation->GetVisibleObjectsCount();
}

TerrainObjectID CTerrainManager::GetVisibleObjectID(size_t index) const
{
	return _implementation->GetVisibleObjectID(index);
}

size_t CTerrainManager::GetNotReadyObjectsInFrustumCount() const
{
	return _implementation->GetNotReadyObjectsInFrustumCount();
}

TerrainObjectID CTerrainManager::GetNotReadyObjectInFrustumID(size_t index) const
{
	return _implementation->GetNotReadyObjectInFrustumID(index);
}


size_t CTerrainManager::GetBoundBoxToBeCalculatedCount() const
{
	return _implementation->GetBoundBoxToBeCalculatedCount();
}

void CTerrainManager::GetTerrainObjectCenter(TerrainObjectID ID, D3DXVECTOR3 * out_pvCenter) const
{
	_implementation->GetTerrainObjectCenter(ID, out_pvCenter);
}

bool CTerrainManager::GetTerrainObjectProjection(TerrainObjectID ID, const D3DXVECTOR3 * in_pvPosFrom, D3DXVECTOR3 * out_pvProjection, D3DXVECTOR3 * out_pvNormal) const
{
	return _implementation->GetTerrainObjectProjection(ID, in_pvPosFrom, out_pvProjection, out_pvNormal);
}

bool CTerrainManager::GetTerrainObjectClosestPoint(TerrainObjectID ID, const D3DXVECTOR3* in_pvPosFrom, D3DXVECTOR3* out_pvClosestPoint, D3DXVECTOR3* out_pvNormal) const
{
	return _implementation->GetTerrainObjectClosestPoint(ID, in_pvPosFrom, out_pvClosestPoint, out_pvNormal);
}

void CTerrainManager::GetTerrainObjectBoundBoxCorners(TerrainObjectID ID, D3DXVECTOR3 out_pvCorners[8]) const
{
	_implementation->GetTerrainObjectBoundBoxCorners(ID, out_pvCorners);
}

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

void CTerrainManager::SetMinDistAlgorithmAccuracy(unsigned int in_uiAccuracy)
{
	_implementation->SetMinDistAlgorithmAccuracy(in_uiAccuracy);
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

//@{ Список объектов для которых нужно загрузить карту высот

//size_t CTerrainManager::GetNewHeightmapsCount() const
//{
//	return _implementation->GetNewHeightmapsCount();
//}
//
//TerrainObjectID CTerrainManager::GetNewHeightmapObjectID(size_t index) const
//{
//	return _implementation->GetNewHeightmapObjectID(index);
//}

//size_t CTerrainManager::GetAwaitingHeightmapsCount() const
//{
//	return _implementation->GetAwaitingHeightmapsCount();
//}
//
//TerrainObjectID CTerrainManager::GetAwaitingHeightmapObjectID(size_t index) const
//{
//	return _implementation->GetAwaitingHeightmapObjectID(index);
//}

//@}


//@}


// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
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
	_pPreliminaryVisibleSubtree = new CTerrainObjectVisibleSubtree(_pTerrainObjectManager);

	_pBoundBoxAsyncManger = new AsyncTaskManager;
}

CTerrainManager::CTerrainManagerImpl::~CTerrainManagerImpl()
{
	DestroyObjects();
	ReleaseTriangulationsAndHeightmaps();

	if (_pTerrainObjectManager)
		delete _pTerrainObjectManager;

	if (_pPreliminaryVisibleSubtree)
		delete _pPreliminaryVisibleSubtree;

	if (_pTerrainVisibility)
		delete _pTerrainVisibility;

	if (_pVisibilityManager)
		delete _pVisibilityManager;

	delete _pBoundBoxAsyncManger;
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

			_globalTerrainShaderParams.aVertexCounts[i][0] = _pTerrainObjectManager->GetLodInfos()[i].AltHeight / _heightfieldCompressionRatio;
			_globalTerrainShaderParams.aVertexCounts[i][1] = _pTerrainObjectManager->GetLodInfos()[i].AltWidth / _heightfieldCompressionRatio;

			if (_pTerrainObjectManager->GetLodInfos()[i].HasBorder)
			{
				_globalTerrainShaderParams.aVertexCounts[i][0] += 1;
				_globalTerrainShaderParams.aVertexCounts[i][1] += 1;
			}


			_globalTerrainShaderParams.aPartitionCoefficients[i][0] = _pTerrainObjectManager->GetLodInfos()[i].CountY;
			_globalTerrainShaderParams.aPartitionCoefficients[i][1] = _pTerrainObjectManager->GetLodInfos()[i].CountX;
		}

	}

	unsigned int uiMaxDepth = std::min<unsigned int>(_pTerrainObjectManager->GetDatabaseInfo().LodCount, in_uiMaxDepth);

	if (in_uiMaxDepth == 0)
		uiMaxDepth = _pTerrainObjectManager->GetDatabaseInfo().LodCount;

	_globalTerrainShaderParams.uiLevelsCount = uiMaxDepth;

	// TODO: Read lods structure

	_wsPlanetRootDirectory = ExtractFileDirectory(wsDbFileName);

	_pTerrainVisibility = new CTerrainVisibility(_pTerrainObjectManager, this, _fWorldScale, 6000000.0f, 0.5, uiResultingMaxDepth);

	_pVisibilityManager = new CVisibilityManager(nullptr, _fWorldScale * 20000000.0f, 1000 * _fWorldScale);
}

void CTerrainManager::CTerrainManagerImpl::SetHeightfieldConverter(HeightfieldConverter * in_pHeightfieldConverter)
{
	_pHeightfieldConverter = in_pHeightfieldConverter;
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
			//_setCachedHFRequest.insert(ID);
		}

	} // unlock mutex

	if (!pHeightfield)
		return nullptr;

	//std::wstring wsHeightmapFileName = _pTerrainObjectManager->GetHeighmapFileName(ID); 

	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	auto objRes = _pTerrainObjectManager->GetObjectHfResolution(ID, _heightfieldCompressionRatio);

	pHeightfield->Config.nCountX = objRes.first;
	pHeightfield->Config.nCountY = objRes.second;

	// считаем данные карты высот из файла
	//_pHeightfieldConverter->ReadHeightfieldDataFromTexture(wsHeightmapFileName.c_str(), *pHeightfield, (unsigned short)_heightfieldCompressionRatio);

	//LogMessage("Loading faceset. Triangulating heightmap '%ls'", wsHeightmapFileName.c_str());

	// заполним граничные данные
	pHeightfield->Config.Coords.fMinLattitude = params.fMinLattitude;
	pHeightfield->Config.Coords.fMaxLattitude = params.fMaxLattitude;
	pHeightfield->Config.Coords.fMinLongitude = params.fMinLongitude;
	pHeightfield->Config.Coords.fMaxLongitude = params.fMaxLongitude;

	pHeightfield->fLattitudeCutCoeff = params.fLattitudeCutCoeff;
	pHeightfield->fLongitudeCutCoeff = params.fLongitudeСutCoeff;

	return pHeightfield;
}

//@{ Список карт высот, которые ожидают вызова команды SetHeightmapReady
//size_t CTerrainManager::CTerrainManagerImpl::GetAwaitingHeightmapsCount() const
//{
//	return _vecAwaitingHeightmaps.size();
//}
//
//TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetAwaitingHeightmapObjectID(size_t index) const
//{
//	return _vecAwaitingHeightmaps[index];
//}
size_t CTerrainManager::CTerrainManagerImpl::GetBoundBoxToBeCalculatedCount() const
{
	return _pBoundBoxAsyncManger->tasksCount();
}
//@}


void CTerrainManager::CTerrainManagerImpl::Update(float in_fDeltaTime)
{
	if (_bRecalculateLodsDistances && _cameraParams.fVFovAngleRad >= 30 * D2R && _cameraParams.uiScreenResolutionY > 0)
	{
		_pTerrainVisibility->CalculateLodDistances(_cameraParams.fVFovAngleRad, _vecLODResolution, _vecLODDiameter,
			_cameraParams.uiScreenResolutionY, _fMaxPixelsPerTexel);

		_bRecalculateLodsDistances = false;
	}

	_vecNewObjectIDs.resize(0);
	_vecObjectsToDelete.resize(0);

	// Обновление списка видимых объектов
	if (_pTerrainVisibility)
		_pTerrainVisibility->UpdateObjectsVisibility(in_fDeltaTime, _cameraParams.vPos * _fWorldScale);

	// Управление временем жизни объектов
	UpdateObjectsLifetime(in_fDeltaTime);

	// поместить на удаление все объекты, которые должны быть удалены немедленно
	ConvertSetToVector(_setObjectsToDelete, _vecObjectsToDelete);
	_setObjectsToDelete.clear();

	// Управление списком объектов на удаление
	ManageDeadObjects();

	// обновить кэш признаков готовности объектов
	UpdateDataReadyStates();
	
	bool bAllReady = true;

	if (!_pTerrainVisibility)
		return;

	for (TerrainObjectID visID : _pTerrainVisibility->GetVisibleObjects())
	{
		CTerrainObject* pTerrainObject = nullptr;
		{
			std::lock_guard<std::mutex> objLock(_objectsMutex);

			auto it = _mapId2Object.find(visID);

			if (it != _mapId2Object.end())
				pTerrainObject = it->second;
		}

		if (!pTerrainObject)
		{
			pTerrainObject = CreateObject(visID);
		}

		if (_bAwaitingVisibleForDataReady)
		{
			if (!pTerrainObject->IsDataReady())
				bAllReady = false;
		}
	}

	// мгновенный видимый во фрустуме набор объектов (могут быть не загружены)
	_vecCurrentVisibleObjsInFrustum = GetObjsInFrustum(_pTerrainVisibility->GetVisibleObjects());

	//@{ подготовить набор неготовых объектов во фрустуме
	_vecNotReadyObjsInFrustum.resize(0);
	for (TerrainObjectID ID : _vecCurrentVisibleObjsInFrustum)
	{
		if (!IsObjectDataReady(ID))
			_vecNotReadyObjsInFrustum.push_back(ID);
	}
	//@}


	size_t nCurrentVisibleInFrustumCount = _vecCurrentVisibleObjsInFrustum.size();

	bool bUsePreliminarySet = true;

	if (bAllReady)
	{
		// Swap visible sets
		_pPreliminaryVisibleSubtree->setToObjects(_pTerrainVisibility->GetVisibleObjects());
	}
	else if (_bAwaitingVisibleForDataReady)
	{
		// Если не весь потенциально видимый набор готов, проверить только часть во фрустуме, и если она готова - выдать ее на выход
		if (IsAllObjectsReady(_vecCurrentVisibleObjsInFrustum))
		{
			bUsePreliminarySet = false;
			_vecReadyVisibleObjects = _vecCurrentVisibleObjsInFrustum;
		}
		else
		{
			UpdatePreliminaryObjects();
		}

	}


	// Расчет списка реально видимых объектов
	if (bUsePreliminarySet)
		CalculateReadyAndVisibleSetAccordingToPreliminary();

	// отсортировать по убыванию номера лода и сказать парентам видимых объектов, чтобы не выгружались
	SortVisibleSetAndPinParents();

	// Сформировать список ожидающих карт высот
	//ConvertSetToVector(_setAwaitingHeightmaps, _vecAwaitingHeightmaps);
	//_setAwaitingHeightmaps.clear();
}

//@{ Список текущих видимых объектов (обязательно загружены)
size_t CTerrainManager::CTerrainManagerImpl::GetNotReadyObjectsInFrustumCount() const
{
	return _vecNotReadyObjsInFrustum.size();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetNotReadyObjectInFrustumID(size_t index) const
{
	return _vecNotReadyObjsInFrustum[index];
}
//@}


void CTerrainManager::CTerrainManagerImpl::UpdatePreliminaryObjects()
{
	_pPreliminaryVisibleSubtree->setLastMaxDepth(_pTerrainVisibility->GetLastMaxDepth());
	_pPreliminaryVisibleSubtree->update(_pTerrainVisibility->GetVisibleObjects(), _setDataReadyObjects);
}

bool CTerrainManager::CTerrainManagerImpl::IsObjectInFrustumAndNotBacksided(TerrainObjectID ID) const
{
	std::vector<vm::Vector3df>* vecRefPoints = nullptr;
	std::vector<vm::Vector3df>* vecRefNormals = nullptr;

	const CTerrainObject* pObj = nullptr;

	auto it = _mapId2Object.find(ID);
	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainManagerImpl::GetObjsInFrustum: unknown object ID: %d", ID);
		return false;
	}

	pObj = it->second;

	pObj->CalculateReferencePoints(&vecRefPoints, &vecRefNormals);

	OrientedBoundBox OBB = pObj->GetOrientedBoundBox();
	if (!_pVisibilityManager->CheckOBBInCamera(ToVisManVec3(OBB._vPos), ToVisManVec3(OBB._vXAxis), ToVisManVec3(OBB._vYAxis), ToVisManVec3(OBB._vZAxis), ToVisManVec3(OBB._vHalfsizes)))
		return false;

	//@{ Проверка объектов на другой стороне Земли
	bool bFullBackSided = true;

	for (size_t i = 0; i < (*vecRefNormals).size(); i++)
	{
		if (vm::dot_prod(normalize((*vecRefPoints)[i] - _cameraParams.vPos), normalize((*vecRefNormals)[i])) < 0)
		{
			bFullBackSided = false;
			break;
		}
	}
	//@}

	return !bFullBackSided;
}

std::vector<TerrainObjectID> CTerrainManager::CTerrainManagerImpl::GetObjsInFrustum(const std::set<TerrainObjectID>& objsToCheck) const
{
	std::vector<TerrainObjectID> result;

	for (TerrainObjectID ID : objsToCheck)
	{
		if (IsObjectInFrustumAndNotBacksided(ID))
			result.push_back(ID);
	}

	return result;
}

bool CTerrainManager::CTerrainManagerImpl::IsObjectDataReady(TerrainObjectID ID) const
{
	std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
	return _setDataReadyObjects.find(ID) != _setDataReadyObjects.end();

	//if (CTerrainObject* pObj = GetTerrainObject(ID))
	//	return pObj->IsDataReady();

	//return false;
}

bool CTerrainManager::CTerrainManagerImpl::IsAllObjectsReady(const std::vector<TerrainObjectID>& vecObjs) const
{
	for (TerrainObjectID ID : vecObjs)
	{
		if (!IsObjectDataReady(ID))
			return false;
	}

	return true;
}

bool CTerrainManager::CTerrainManagerImpl::IsDataReady(TerrainObjectID ID) const
{
	return IsObjectDataReady(ID);
}

void CTerrainManager::CTerrainManagerImpl::UpdateDataReadyStates()
{
	std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
	_setDataReadyObjects.clear();

	for (auto it = _mapId2Object.begin(); it != _mapId2Object.end(); it++)
	{
		if (it->second->IsDataReady())
			_setDataReadyObjects.insert(it->first);
	}
}

// Удаление объектов из списка "на удаление"
void CTerrainManager::CTerrainManagerImpl::ManageDeadObjects()
{
	for (TerrainObjectID deadObj : _vecObjectsToDelete)
	{
		// Триангуляция помечается как "ненужная" и удалиться по истечении некоторого времени, если ее статус не изменится
		{
			std::lock_guard<std::mutex> lock(_objectTriangulationsMutex);

			auto itTri = _mapObjectTriangulations.find(deadObj);
			if (itTri == _mapObjectTriangulations.end())
			{
				LogMessage("Object %d has no triangulation while was alive", deadObj);
				//continue;
			}
			else
				itTri->second._alive = false;
		}

		// Объект удаляется из списка объектов
		{
			std::lock_guard<std::mutex> objectsLock(_objectsMutex);

			auto it = _mapId2Object.find(deadObj);
			if (it != _mapId2Object.end())
			{
				CTerrainObject* pObject = it->second;

				delete pObject;

				_mapId2Object.erase(it);
			}
		}

		// Удалить из списка готовых объектов
		/*{
			std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
			_setDataReadyObjects.erase(deadObj);
		}*/
	}
}

bool CTerrainManager::CTerrainManagerImpl::UpdateTriangulations()
{
	if (!_pHeightfieldConverter)
		return false;

//	_setAwaitingHeightmaps.clear();

	static std::vector<std::pair<TerrainObjectID, SObjectTriangulation*>> s_vecTriangulationsToCreate;
	s_vecTriangulationsToCreate.resize(0);

	// lock
	{
		std::lock_guard<std::mutex> contLock(_containersMutex);

		for (auto it = _setNotReadyTriangulations.begin(); it != _setNotReadyTriangulations.end(); )
		{
			//if (s_vecTriangulationsToCreate.size() >= MAX_TRIANGULATIONS_PER_FRAME)
			//	break;

			TerrainObjectID ID = *it;
			CTerrainObject* pInternalObject = nullptr;

			{
				std::lock_guard<std::mutex> objLock(_objectsMutex);

				auto itObj = _mapId2Object.find(ID);
				if (itObj == _mapId2Object.end())
					pInternalObject = nullptr;
				else
					pInternalObject = itObj->second;
			}

			if (!pInternalObject)
			{
				//LogMessage("Creating triangulation for deleted object id: %d, removing", ID);

				// Если объект уже удален, то стирать его из списка на триангуляцию
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

				/*if (pInternalObject->IsDataReady())
				{
					std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
					_setDataReadyObjects.insert(ID);
				}*/

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

		_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

		bool someNotReadyHF = false;

		SHeightfield* pHeightfield = RequestObjectHeightfield(ID);

		if (!pHeightfield)
		{
			someNotReadyHF = true;
			//_setAwaitingHeightmaps.insert(ID);
		}

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
					if (_bEnabledBorderNormals)
						someNotReadyHF = true;
				}
			}
		}

		if (someNotReadyHF)
			continue;

		// Создадим триангуляцию с помощью ComputeShader. В объекте _triangulation лежат индексные и вертексные буферы
		_pHeightfieldConverter->CreateTriangulationImmediate(pHeightfield, params.fLongitudeСutCoeff, params.fLattitudeCutCoeff, &objTri._triangulation, neighbourHeightfields);

		objTri._ready = true;

		_setNotReadyTriangulations.erase(ID);

		std::lock_guard<std::mutex> objLock(_objectsMutex);
		auto it = _mapId2Object.find(ID);

		if (it != _mapId2Object.end())
		{
			it->second->SetTriangulationReady(&objTri._triangulation);

			/*if (it->second->IsDataReady())
			{
				std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
				_setDataReadyObjects.insert(ID);
			}*/
		}
		else
			objTri._alive = false;
	}


	UpdateTriangulationsAndHeightfieldLifetime();

	//_vecHeightmapsToCreate.resize(0);
	//for (TerrainObjectID ID : _setCachedHFRequest)
	//	_vecHeightmapsToCreate.push_back(ID);

	//// TODO: remove not needed any more HF from here
	//_setCachedHFRequest.clear();

	return true;
}

void CTerrainManager::CTerrainManagerImpl::UpdateObjectsLifetime(float in_fDeltatime)
{
	if (!_pTerrainVisibility)
		return;

	std::set<TerrainObjectID> setVisibleObjs = _pTerrainVisibility->GetVisibleObjects();

	for (auto it = _mapId2Object.begin(); it != _mapId2Object.end();)
	{
		TerrainObjectID ID = it->first;
		CTerrainObject* pObj = it->second;

		bool bAlive = false;

		auto itVisIt = setVisibleObjs.find(ID);

		if (_pPreliminaryVisibleSubtree->hasObject(ID) ||
			itVisIt != setVisibleObjs.end())
		{
			bAlive = true;
		}

		if (!bAlive)
		{
			if (pObj->timeSinceUnused > 1.f)
				DestroyObject(ID);

			pObj->timeSinceUnused += in_fDeltatime;
		}
		else
			pObj->timeSinceUnused = 0;

		it++;
	}
}

void CTerrainManager::CTerrainManagerImpl::CalculateReadyAndVisibleSetAccordingToPreliminary()
{
	_vecReadyVisibleObjects = GetObjsInFrustum(_pPreliminaryVisibleSubtree->objects());
}

void CTerrainManager::CTerrainManagerImpl::PinObject(TerrainObjectID ID)
{
	if (CTerrainObject* pObj = GetTerrainObject(ID))
		pObj->timeSinceUnused = 0;
}

void CTerrainManager::CTerrainManagerImpl::PinParents(TerrainObjectID ID)
{
	TerrainObjectID parentID = GetTerrainObjectParent(ID);
	while (parentID != INVALID_TERRAIN_OBJECT_ID)
	{
		PinObject(parentID);
		parentID = GetTerrainObjectParent(parentID);
	}
}

void CTerrainManager::CTerrainManagerImpl::SortVisibleSetAndPinParents()
{
	std::vector<TerrainObjectID> vecTerrainObjectParentsChildren;
	// pin parents
	for (TerrainObjectID ID : _vecReadyVisibleObjects)
	{
		TerrainObjectID parentID = GetTerrainObjectParent(ID);

		if (parentID != INVALID_TERRAIN_OBJECT_ID)
		{
			PinObject(parentID);
			PinParents(parentID);

			vecTerrainObjectParentsChildren.resize(0);
			_pTerrainObjectManager->GetTerrainObjectChildren(parentID, vecTerrainObjectParentsChildren);

			for (TerrainObjectID childID : vecTerrainObjectParentsChildren)
				PinObject(childID);
		}
	}

	for (TerrainObjectID rootID : _pTerrainObjectManager->GetRootObjects())
	{
		PinObject(rootID);
	}

	// sort by decreasing of lod level
	auto sortFunc = [this](TerrainObjectID obj1, TerrainObjectID obj2) -> bool
	{
		return _pTerrainObjectManager->GetObjectDepth(obj1) > _pTerrainObjectManager->GetObjectDepth(obj2);
	};

	std::sort(_vecReadyVisibleObjects.begin(), _vecReadyVisibleObjects.end(), sortFunc);
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




void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams* out_pParams) const
{
	std::lock_guard<std::mutex> lock(_objectsMutex);

	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end() || it->second == nullptr)
	{
		_pTerrainObjectManager->ComputeTerrainObjectParams(ID, *out_pParams);
		return;
	}

	*out_pParams = (*it->second->GetParams());
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation ** out_ppTriangulation) 
{
	std::lock_guard<std::mutex> triLock(_objectTriangulationsMutex);

	auto it = _mapObjectTriangulations.find(ID);

	if (it == _mapObjectTriangulations.end())
	{
		*out_ppTriangulation = nullptr;
		return;
	}

	if (!it->second._ready)
	{
		*out_ppTriangulation = nullptr;
		return;
	}

	it->second._timeSinceDead = 0;
	*out_ppTriangulation = &(it->second._triangulation);

}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8])
{
	_pTerrainObjectManager->GetTerrainObjectNeighbours(ID, outNeighbours);
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetTerrainObjectParent(TerrainObjectID ID) const
{
	return _pTerrainObjectManager->GetTerrainObjectParent(ID);
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

// получить имя текстуры для данного объекта
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

// получить имя карты высот для данного объекта
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

void  CTerrainManager::CTerrainManagerImpl::CorrectObjectPointAccordingToCachedMidHeight(TerrainObjectID ID, vm::Vector3df& io_vPoint) const
{
	double dfCorrectionCoeff = 1;

	{
		std::lock_guard<std::mutex> lock(_mapCachedMidHeightMutex);

		auto it = _mapCachedMidHeght.find(ID);
		if (it != _mapCachedMidHeght.end())
		{
			vm::Vector3df vNonScaledPoint = io_vPoint / _fWorldScale;
			double dfCurHeight = GetWGS84Height(vNonScaledPoint);
			double dfCurRadius = GetWGS84Radius(vNonScaledPoint);

			dfCorrectionCoeff = (dfCurRadius + it->second) / (dfCurHeight + dfCurRadius);
		}
		else
			return;
	}

	io_vPoint = io_vPoint * dfCorrectionCoeff;
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectCenter(TerrainObjectID ID, D3DXVECTOR3 * out_pvCenter) const
{
	if (!out_pvCenter)
		return;

	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);

	vm::Vector3df vPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLat) * _fWorldScale;

	CorrectObjectPointAccordingToCachedMidHeight(ID, vPoint);

	out_pvCenter->x = vPoint[0];
	out_pvCenter->y = vPoint[1];
	out_pvCenter->z = vPoint[2];
}

CTerrainObject*	CTerrainManager::CTerrainManagerImpl::GetTerrainObject(TerrainObjectID ID, const std::string& assertOwner) const
{
	CTerrainObject* pObj = nullptr;
	{
		std::lock_guard<std::mutex> lock(_objectsMutex);
		auto it = _mapId2Object.find(ID);

		if (it != _mapId2Object.end())
		{
			pObj = it->second;
		}
	}

	if (!pObj && !assertOwner.empty())
	{
		LogMessage("Error! %s, object ID %d is not exist.", assertOwner.c_str(), ID);
		return nullptr;
	}

	return pObj;
}

bool CTerrainManager::CTerrainManagerImpl::GetTerrainObjectProjection(TerrainObjectID ID, const vm::Vector3df& in_vPosFrom, vm::Vector3df& out_vProjection, vm::Vector3df& out_vNormal) const
{
	if (CTerrainObject* pObj = GetTerrainObject(ID))
		return pObj->CalculateProjectionOnSurface(in_vPosFrom, out_vProjection, out_vNormal);

	double dfLong, dfLat;
	bool isPositionAboveBlock = GetObjectManager()->GetClippedProjection(ID, in_vPosFrom, dfLat, dfLong);

	out_vProjection = GetWGS84SurfacePoint(dfLong, dfLat) * _pHeightfieldConverter->GetWorldScale();
	out_vNormal = GetWGS84SurfaceNormal(dfLong, dfLat);

	CorrectObjectPointAccordingToCachedMidHeight(ID, out_vProjection);

	return isPositionAboveBlock;
}

bool CTerrainManager::CTerrainManagerImpl::GetTerrainObjectProjection(TerrainObjectID ID, const D3DXVECTOR3 * in_pvPosFrom, D3DXVECTOR3 * out_pvProjection, D3DXVECTOR3* out_pvNormal) const
{
	vm::Vector3df vProjection(0, 0, 0), vNormal(1, 0, 0);
	bool result = GetTerrainObjectProjection(ID, vm::Vector3df(in_pvPosFrom->x, in_pvPosFrom->y, in_pvPosFrom->z), vProjection, vNormal);

	if (out_pvProjection)
	{
		out_pvProjection->x = vProjection[0];
		out_pvProjection->y = vProjection[1];
		out_pvProjection->z = vProjection[2];
	}

	if (out_pvNormal)
	{
		out_pvNormal->x = vNormal[0];
		out_pvNormal->y = vNormal[1];
		out_pvNormal->z = vNormal[2];
	}

	return result;
}

vm::Vector3df CTerrainManager::CTerrainManagerImpl::GetTerrainObjectCenter(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);

	vm::Vector3df vPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLat);

	return vPoint;
}

double CTerrainManager::CTerrainManagerImpl::GetTerrainObjectDiameter(TerrainObjectID ID) const
{
	if (CTerrainObject* pObj = GetTerrainObject(ID))
	{
		return pObj->GetOrientedBoundBox().diameter() / _pHeightfieldConverter->GetWorldScale();
	}

	STerrainBlockParams params;

	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	double dfMidLat = 0.5 * (dfMinLat + dfMaxLat);
	double dfMidLong = 0.5 * (dfMinLong + dfMaxLong);

	vm::Vector3df vRefPoint = GetWGS84SurfacePoint(dfMidLong, dfMidLong);
	double dfMidAngle = 0.5*(fabs(dfMaxLat - dfMinLat) + fabs(dfMaxLong - dfMinLong));

	return dfMidAngle * vm::length(vRefPoint);
}

void CTerrainManager::CTerrainManagerImpl::CacheTerrainObjectMidHeight(TerrainObjectID ID, double height)
{
	std::lock_guard<std::mutex> lock(_mapCachedMidHeightMutex);
	_mapCachedMidHeght[ID] = height;
}


bool CTerrainManager::CTerrainManagerImpl::GetTerrainObjectClosestPoint(TerrainObjectID ID, const vm::Vector3df& in_pvPosFrom, vm::Vector3df& out_pvClosestPoint, vm::Vector3df& out_pvNormal) const
{
	if (CTerrainObject* pObj = GetTerrainObject(ID))
		return pObj->CalculateClosestPoint(in_pvPosFrom, out_pvClosestPoint, out_pvNormal);

	double dfLong, dfLat;
	bool isPositionAboveBlock = GetObjectManager()->GetClippedProjection(ID, in_pvPosFrom, dfLat, dfLong);

	out_pvClosestPoint = GetWGS84SurfacePoint(dfLong, dfLat) * _pHeightfieldConverter->GetWorldScale();
	out_pvNormal = GetWGS84SurfaceNormal(dfLong, dfLat);

	CorrectObjectPointAccordingToCachedMidHeight(ID, out_pvClosestPoint);

	return isPositionAboveBlock;
}

bool  CTerrainManager::CTerrainManagerImpl::GetTerrainObjectClosestPoint(TerrainObjectID ID, const D3DXVECTOR3* in_pvPosFrom, D3DXVECTOR3* out_pvClosestPoint, D3DXVECTOR3* out_pvNormal) const
{
	vm::Vector3df vClosest(0, 0, 0), vNormal(1, 0, 0);
	bool result = GetTerrainObjectClosestPoint(ID, vm::Vector3df(in_pvPosFrom->x, in_pvPosFrom->y, in_pvPosFrom->z), vClosest, vNormal);

	if (out_pvClosestPoint)
	{
		out_pvClosestPoint->x = vClosest[0];
		out_pvClosestPoint->y = vClosest[1];
		out_pvClosestPoint->z = vClosest[2];
	}

	if (out_pvNormal)
	{
		out_pvNormal->x = vNormal[0];
		out_pvNormal->y = vNormal[1];
		out_pvNormal->z = vNormal[2];
	}

	return result;
}

void CTerrainManager::CTerrainManagerImpl::GetTerrainObjectBoundBoxCorners(TerrainObjectID ID, D3DXVECTOR3 out_pvCorners[8]) const
{
	if (CTerrainObject* pObj = GetTerrainObject(ID, "GetTerrainObjectBoundBoxCorners"))
		pObj->GetBoundBoxCorners(out_pvCorners);
}

void CTerrainManager::CTerrainManagerImpl::SetTextureReady(TerrainObjectID ID)
{
	std::lock_guard<std::mutex> lock(_objectsMutex);
	auto it = _mapId2Object.find(ID);

	if (it != _mapId2Object.end())
	{
		it->second->SetTextureReady();

		/*if (it->second->IsDataReady())
		{
			std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
			_setDataReadyObjects.insert(ID);
		}*/
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

void CTerrainManager::CTerrainManagerImpl::SetMinDistAlgorithmAccuracy(unsigned int in_uiAccuracy)
{
	_uiMinDistAlgorithmAccuracy = in_uiAccuracy;

	if (_uiMinDistAlgorithmAccuracy == 0)
	{
		LogMessage("CTerrainManagerImpl::SetMinDistAlgorithmAccuracy: in_uiAccuracy cannot be == 0, setting to 1.");
		_uiMinDistAlgorithmAccuracy = 1;
	}
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

	out_BB.update(GetTransformedPoint(p1, vPos, vX, vY, vZ));
	out_BB.update(GetTransformedPoint(p2, vPos, vX, vY, vZ));
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

	vm::BoundBox<double> vBoundBox(vm::Vector3f(0, 0, 0));

	double dfMinHeight =  -200.0;// -10000.0;
	double dfMaxHeight = 9000.0;// 20000.0;

	/*if (_pHeightfieldConverter)
	{
		dfMinHeight *= _pHeightfieldConverter->GetHeightScale();
		dfMaxHeight *= _pHeightfieldConverter->GetHeightScale();
	}*/

	double dfMinLat = in_Coords.fMinLattitude;
	double dfMaxLat = in_Coords.fMaxLattitude;
	double dfMinLong = in_Coords.fMinLongitude;
	double dfMaxLong = in_Coords.fMaxLongitude;

	const int N = 10;

	double dfDeltaLat = (dfMaxLat - dfMinLat) / N;
	double dfDeltaLong = (dfMaxLong - dfMinLong) / N;

	double dfLat = dfMinLat;

	for (int i = 0; i < N + 1; i++)
	{
		double dfLong = dfMinLong;

		for (int j = 0; j < N + 1; j++)
		{
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight, vMiddlePoint, vEast, vNormal, vNorth);
			UpdateBBoxSurfacePoint(vBoundBox, dfLong, dfLat, dfMinHeight, dfMaxHeight, vMiddlePoint, vEast, vNormal, vNorth);

			dfLong += dfDeltaLong;
		}
		
		dfLat += dfDeltaLat;
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

////@{ Список объектов для которых нужно загрузить карту высот
//
//size_t CTerrainManager::CTerrainManagerImpl::GetNewHeightmapsCount() const
//{
//	return _vecHeightmapsToCreate.size();
//}
//
//TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetNewHeightmapObjectID(size_t index) const
//{
//	return _vecHeightmapsToCreate[index];
//}
//
////@}


CTerrainObject* CTerrainManager::CTerrainManagerImpl::CreateObject(TerrainObjectID ID)
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

	CTerrainObject* pObject = new CTerrainObject(this, ID, params, coordsInfo, _pTerrainObjectManager->GetTextureFileName(ID), 
		_pTerrainObjectManager->GetHeighmapFileName(ID), _pHeightfieldConverter, _pBoundBoxAsyncManger);

	{
		std::lock_guard<std::mutex> objLock(_objectsMutex);
		_mapId2Object[ID] = pObject;
	}

	if (!_pHeightfieldConverter)
	{
		pObject->SetTriangulationReady(nullptr);

		/*if (pObject->IsDataReady())
		{
			std::lock_guard<std::mutex> objectsLock(_dataReadyMutex);
			_setDataReadyObjects.insert(ID);
		}*/
	}

	_vecNewObjectIDs.push_back(pObject->GetID());

	std::lock_guard<std::mutex> lock(_containersMutex);
	_setNotReadyTriangulations.insert(pObject->GetID());

	return pObject;
}

void CTerrainManager::CTerrainManagerImpl::DestroyObject(TerrainObjectID ID)
{
	//_setPreliminaryObjectsToDelete.insert(ID);
	_setObjectsToDelete.insert(ID);
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

		vecObjHF.resize(0);
		vecObjT.resize(0);

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

//@{ Функции установки линейки расстояний лодов

// Установить линейку расстояний для NLods лодов
void CTerrainManager::CTerrainManagerImpl::SetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	if (_pTerrainVisibility)
		_pTerrainVisibility->SetLodDistancesKM(aLodDistances, NLods);
}

// Считать линейку расстояний для NLods лодов
void CTerrainManager::CTerrainManagerImpl::GetLodDistancesKM(double* aLodDistances, size_t NLods)
{
	if (_pTerrainVisibility)
		_pTerrainVisibility->GetLodDistancesKM(aLodDistances, NLods);
}

// Рассчитать автоматически линейку расстояний исходя из максимального количества пикселей на тексель
// (учитываются: FOV камеры, разрешение экрана, размер текстур лодов, линейные размеры соответствующих блоков Земли)
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


//@{ Функции получения параметров для шейдеров

// заполнить структуру с глобальными шейдерными параметрами
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

bool CTerrainManager::CTerrainManagerImpl::IsObjectPotentiallyVisible(TerrainObjectID ID) const
{
	return _pPreliminaryVisibleSubtree->hasObject(ID) ||
		std::find(_vecReadyVisibleObjects.begin(), _vecReadyVisibleObjects.end(), ID) != _vecReadyVisibleObjects.end();
}

// заполнить структуру с параметрами для указанного блока
void CTerrainManager::CTerrainManagerImpl::FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams)
{
	STerrainBlockParams params;
	_pTerrainObjectManager->ComputeTerrainObjectParams(ID, params);

	SHeightfield* pHeightfield = RequestObjectHeightfield(ID);

	if (!pHeightfield)
	{
//		LogMessage("Error in FillTerrainBlockShaderParams: cannot find heightfield for object %d", ID);
		return;
	}

	TerrainObjectID neighbours[8];
	GetTerrainObjectNeighbours(ID, neighbours);

	const SHeightfield* neighbourHeightfields[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	for (int i = 0; i < 8; i++)
	{
		if (neighbours[i] != INVALID_TERRAIN_OBJECT_ID)
		{
			/*if (_pPreliminaryVisibleSubtree->hasObject(neighbours[i]) &&
				(_pTerrainObjectManager->GetObjectDepth(neighbours[i]) != 0))
			{
				neighbours[i] = _pTerrainObjectManager->GetTerrainObjectParent(neighbours[i]);

				if (_pPreliminaryVisibleSubtree->hasObject(neighbours[i]))
					neighbours[i] = INVALID_TERRAIN_OBJECT_ID;
			}*/

			while (!IsObjectPotentiallyVisible(neighbours[i]) &&
				(_pTerrainObjectManager->GetObjectDepth(neighbours[i]) != 0))
			{
				neighbours[i] = _pTerrainObjectManager->GetTerrainObjectParent(neighbours[i]);
			}

			if (!IsObjectPotentiallyVisible(neighbours[i]))
				neighbours[i] = INVALID_TERRAIN_OBJECT_ID;

			if (neighbours[i] != INVALID_TERRAIN_OBJECT_ID)
				neighbourHeightfields[i] = RequestObjectHeightfield(neighbours[i]);
		}
	}

	out_pTerrainBlockShaderParams->pHeightfield = pHeightfield->pTextureSRV;

	STriangulation* objTri = nullptr;
	GetTerrainObjectTriangulation(ID, &objTri);

	if (objTri)
		out_pTerrainBlockShaderParams->pVertexBuffer = objTri->pVertexBuffer;

	out_pTerrainBlockShaderParams->fMinLattitude = params.fMinLattitude;
	out_pTerrainBlockShaderParams->fMaxLattitude = params.fMaxLattitude;
												   
	out_pTerrainBlockShaderParams->fMinLongitude = params.fMinLongitude;
	out_pTerrainBlockShaderParams->fMaxLongitude = params.fMaxLongitude;

	out_pTerrainBlockShaderParams->nCountX = pHeightfield->Config.nCountX * params.fLattitudeCutCoeff;
	out_pTerrainBlockShaderParams->nCountY = pHeightfield->Config.nCountY * params.fLongitudeСutCoeff;

	if (out_pTerrainBlockShaderParams->nCountX < 2)
		out_pTerrainBlockShaderParams->nCountX = 2;

	if (out_pTerrainBlockShaderParams->nCountY < 2)
		out_pTerrainBlockShaderParams->nCountY = 2;



	out_pTerrainBlockShaderParams->fLongitudeCoeff = params.fLongitudeСutCoeff;
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
