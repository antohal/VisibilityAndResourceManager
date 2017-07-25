#include "TerrainManagerImpl.h"
#include "Log.h"
#include "FileUtil.h"
#include "wgs84.h"

#include <d3dx10math.h>

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

//@}


// �������� ��������� �� �������� �������� (���� ���������� ������ ���������� ������������� ��������� ��������, �������� �� �������� ��-���������)
CResourceManager* CTerrainManager::GetResourceManager()
{
	return _implementation->GetResourceManager();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//		CTerrainManager::CTerrainManagerImpl
//-------------------------------------------------------------------------------------------------------------------------------------------------------

CTerrainManager::CTerrainManagerImpl::~CTerrainManagerImpl()
{
	if (_pResourceManager)
		delete _pResourceManager;

	if (_pVisibilityManager)
		delete _pVisibilityManager;

	DestroyObjects();

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


	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;
	pTerrainVisibilityManager->Init(this, _fWorldScale, 6000000.0f, 0.5, uiMaxDepth);

	_pVisibilityManager->InstallPlugin(pTerrainVisibilityManager);
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

	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;
	pTerrainVisibilityManager->Init(this, _fWorldScale, 6000000.0f, 0.5f, depth);

	_pVisibilityManager->InstallPlugin(pTerrainVisibilityManager);
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

	vUp.x = (float)in_vDir->x;
	vUp.y = (float)in_vDir->y;
	vUp.z = (float)in_vDir->z;

	_pVisibilityManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
	_pResourceManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
}

void CTerrainManager::CTerrainManagerImpl::Update(float in_fDeltaTime)
{
	_containersMutex.lock();

	_vecNewObjectIDs.clear();
	_vecObjectsToDelete.clear();

	_containersMutex.unlock();

	_pVisibilityManager->UpdateVisibleObjectsSet();
	_pResourceManager->Update(in_fDeltaTime);
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
	return _pVisibilityManager->GetVisibleObjectsCount();
}

TerrainObjectID CTerrainManager::CTerrainManagerImpl::GetVisibleObjectID(size_t index) const
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(_pVisibilityManager->GetVisibleObjectPtr(index));

	if (!pTerrainObject)
		return -1;

	return pTerrainObject->GetID();
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

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecNewObjectIDs.push_back(pTerrainObject->GetID());
}

void CTerrainManager::CTerrainManagerImpl::RequestUnloadResource(C3DBaseResource* in_pResource)
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(in_pResource);

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecObjectsToDelete.push_back(pTerrainObject->GetID());
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


void CTerrainManager::CTerrainManagerImpl::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords)
{
	double middleLattitude = (in_Coords.fMinLattitude + in_Coords.fMaxLattitude)*0.5;
	double middleLongitude = (in_Coords.fMinLongitude + in_Coords.fMaxLongitude)*0.5;

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

	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMinLongitude, in_Coords.fMinLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMaxLongitude, in_Coords.fMinLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMinLongitude, in_Coords.fMaxLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMaxLongitude, in_Coords.fMaxLattitude, dfMinHeight, dfMaxHeight);

	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMaxLongitude, middleLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, in_Coords.fMinLongitude, middleLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, middleLongitude, in_Coords.fMinLattitude, dfMinHeight, dfMaxHeight);
	UpdateBBoxSurfacePoint(vBoundBox, middleLongitude, in_Coords.fMaxLattitude, dfMinHeight, dfMaxHeight);


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
	ComputeTriangulationCoords(coords, coordsInfo);

	CInternalTerrainObject* pObject = new CInternalTerrainObject(static_cast<C3DBaseManager*>(this), _idCurrentIDForNewObject, in_pData, coordsInfo);

	_vecObjects.push_back(pObject);
	_mapId2Object[_idCurrentIDForNewObject] = pObject;

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
