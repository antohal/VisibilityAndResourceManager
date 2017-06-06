#include "TerrainObjectManagerImpl.h"
#include "Log.h"

#include <d3dx10math.h>

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

CTerrainObjectManager::CTerrainObjectManager()
{
	LogInit("TerrainManager.log");
	LogEnable(true);

	_implementation = new CTerrainObjectManagerImpl;
}

CTerrainObjectManager::~CTerrainObjectManager()
{
	delete _implementation;
}
// инициализация. Параметр - имя дериктории, где лежат данные Земли
void CTerrainObjectManager::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fHeightScale)
{
	_implementation->Init(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, in_fWorldScale, in_fHeightScale);
}

void CTerrainObjectManager::InitGenerated(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fHeightScale)
{
	_implementation->InitGenerated(in_pD3DDevice11, in_pDeviceContext, in_pcwszPlanetDirectory, N, M, depth, in_fWorldScale, in_fHeightScale);
}

void CTerrainObjectManager::SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection)
{
	_implementation->SetViewProjection(in_vPos, in_vDir, in_vUp, in_pmProjection);
}

// В момент вызова этой функции формируется 4 списка: 
// объекты, которые нужно создать
// объекты, которые стали видимыми
// объекты, которые стали невидимыми
// объекты, которые нужно удалить
void CTerrainObjectManager::Update(float in_fDeltaTime)
{
	_implementation->Update(in_fDeltaTime);
}

//Получить описание объекта Земли по идентификатору
const CTerrainBlockDesc*	CTerrainObjectManager::GetTerrainObjectDesc(TerrainObjectID ID) const
{
	return _implementation->GetTerrainObjectDesc(ID);
}

//@{ Список новых объектов, которые нужно создать (могут стать видимыми)
size_t CTerrainObjectManager::GetNewObjectsCount() const
{
	return _implementation->GetNewObjectsCount();
}

TerrainObjectID	CTerrainObjectManager::GetNewObjectID(size_t index) const
{
	return _implementation->GetNewObjectID(index);
}
//@}


//@{ Список объектов, которые нужно удалить (выпали из списка потенциально видимых)
size_t CTerrainObjectManager::GetObjectsToDeleteCount() const
{
	return _implementation->GetObjectsToDeleteCount();
}

TerrainObjectID CTerrainObjectManager::GetObjectToDeleteID(size_t index) const
{
	return _implementation->GetObjectToDeleteID(index);
}

//@}

//@{ Список текущих видимых объектов
size_t CTerrainObjectManager::GetVisibleObjectsCount() const
{
	return _implementation->GetVisibleObjectsCount();
}

TerrainObjectID CTerrainObjectManager::GetVisibleObjectID(size_t index) const
{
	return _implementation->GetVisibleObjectID(index);
}

//@}


// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
CResourceManager* CTerrainObjectManager::GetResourceManager()
{
	return _implementation->GetResourceManager();
}

HeightfieldConverter * CTerrainObjectManager::GetHeightfieldConverter()
{
	return _implementation->GetHeightfieldConverter();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//		CTerrainObjectManager::CTerrainObjectManagerImpl
//-------------------------------------------------------------------------------------------------------------------------------------------------------

CTerrainObjectManager::CTerrainObjectManagerImpl::~CTerrainObjectManagerImpl()
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

	if (_pHeightfieldConverter)
		delete _pHeightfieldConverter;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t * in_pcwszPlanetDirectory, float in_fWorldScale, float in_fHeightScale)
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pHeightfieldConverter = new HeightfieldConverter();
	_pResourceManager = new CResourceManager();

	_pHeightfieldConverter->Init(in_pD3DDevice11, in_pDeviceContext);

	_pHeightfieldConverter->SetWorldScale(in_fWorldScale);
	_pHeightfieldConverter->SetHeightScale(in_fHeightScale);

	_fWorldScale = in_fWorldScale;

	LogMessage("Loading planet terrain info");

	unsigned int uiMaxDepth = 0;
	_pTerrainDataManager->LoadTerrainDataInfo(in_pcwszPlanetDirectory, &_pPlanetTerrainData, &uiMaxDepth);

	CreateObjects();

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());
	_pResourceManager->AddVisibilityManager(_pVisibilityManager);


	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;
	pTerrainVisibilityManager->Init(this, in_fWorldScale, 6000000.0f, 0.5, uiMaxDepth);

	_pVisibilityManager->InstallPlugin(pTerrainVisibilityManager);
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fHeightScale)
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pHeightfieldConverter = new HeightfieldConverter();
	_pResourceManager = new CResourceManager();

	_pHeightfieldConverter->Init(in_pD3DDevice11, in_pDeviceContext);

	_pHeightfieldConverter->SetWorldScale(in_fWorldScale);
	_pHeightfieldConverter->SetHeightScale(in_fHeightScale);

	_fWorldScale = in_fWorldScale;

	LogMessage("Generating planet terrain info");

	_pTerrainDataManager->GenerateTerrainDataInfo(in_pcwszPlanetDirectory, &_pPlanetTerrainData, M, N, depth);

	CreateObjects();

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());
	_pResourceManager->AddVisibilityManager(_pVisibilityManager);

	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;
	pTerrainVisibilityManager->Init(this, in_fWorldScale, 6000000.0f, 0.5f, depth);

	_pVisibilityManager->InstallPlugin(pTerrainVisibilityManager);
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::SetViewProjection(const D3DXVECTOR3 & in_vPos, const D3DXVECTOR3 & in_vDir, const D3DXVECTOR3 & in_vUp, const D3DMATRIX * in_pmProjection)
{
	Vector3 vPos, vDir, vUp;

	vPos.x = (float)in_vPos.x;
	vPos.y = (float)in_vPos.y;
	vPos.z = (float)in_vPos.z;

	vDir.x = (float)in_vDir.x;
	vDir.y = (float)in_vDir.y;
	vDir.z = (float)in_vDir.z;

	vUp.x = (float)in_vDir.x;
	vUp.y = (float)in_vDir.y;
	vUp.z = (float)in_vDir.z;

	_pVisibilityManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
	_pResourceManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::Update(float in_fDeltaTime)
{
	_containersMutex.lock();

	_vecNewObjectIDs.clear();
	_vecObjectsToDelete.clear();

	_containersMutex.unlock();

	_pVisibilityManager->UpdateVisibleObjectsSet();
	_pResourceManager->Update(in_fDeltaTime);
}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetTerrainObjectDesc(TerrainObjectID ID) const
{
	auto it = _mapId2Object.find(ID);

	if (it == _mapId2Object.end())
	{
		LogMessage("CTerrainObjectManager::CTerrainObjectManagerImpl::GetTerrainObjectDesc: cannot find object by ID = %d", ID);
		return nullptr;
	}

	return it->second->GetDesc();
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewObjectsCount() const
{
	std::lock_guard<std::mutex> lock(_containersMutex);
	
	return _vecNewObjectIDs.size();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewObjectID(size_t index) const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

	if (index >= _vecNewObjectIDs.size())
	{
		LogMessage("CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewObjectID: Illegal object id (%d), objects count = %d", index, _vecNewObjectIDs.size());
		return -1;
	}

	return _vecNewObjectIDs[index];
}

//size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewVisibleObjectsCount() const
//{
//	return size_t();
//}
//
//TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewVisibleObjectID(size_t index) const
//{
//	return TerrainObjectID();
//}
//
//size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewInvisibleObjectsCount() const
//{
//	return size_t();
//}
//
//TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewInvisibleObjectID(size_t index)
//{
//	return TerrainObjectID();
//}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectsToDeleteCount() const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

	return _vecObjectsToDelete.size();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectToDeleteID(size_t index) const
{
	std::lock_guard<std::mutex> lock(_containersMutex);

	if (index >= _vecObjectsToDelete.size())
	{
		LogMessage("CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectToDeleteID: Illegal object id (%d), objects count = %d", index, _vecObjectsToDelete.size());
		return -1;
	}

	return _vecObjectsToDelete[index];
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleObjectsCount() const
{
	return _pVisibilityManager->GetVisibleObjectsCount();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleObjectID(size_t index) const
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(_pVisibilityManager->GetVisibleObjectPtr(index));

	if (!pTerrainObject)
		return -1;

	return pTerrainObject->GetID();
}

CResourceManager * CTerrainObjectManager::CTerrainObjectManagerImpl::GetResourceManager()
{
	return _pResourceManager;
}

HeightfieldConverter * CTerrainObjectManager::CTerrainObjectManagerImpl::GetHeightfieldConverter()
{
	return _pHeightfieldConverter;
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectsCount() const
{
	return _vecObjects.size();
}

C3DBaseObject*	CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectByIndex(size_t id) const
{
	if (id >= _vecObjects.size())
	{
		LogMessage("CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectByIndex: Illegal object id (%d), objects count = %d", id, _vecObjects.size());
		return nullptr;
	}

	return _vecObjects[id];
}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetTerrainDataForObject(C3DBaseObject * pObject) const
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(pObject);

	return pTerrainObject->GetDesc();
}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetRootTerrainData() const
{
	return _pPlanetTerrainData;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::RequestLoadResource(C3DBaseResource* in_pResource)
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(in_pResource);

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecNewObjectIDs.push_back(pTerrainObject->GetID());
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::RequestUnloadResource(C3DBaseResource* in_pResource)
{
	CInternalTerrainObject* pTerrainObject = static_cast<CInternalTerrainObject*>(in_pResource);

	std::lock_guard<std::mutex> lock(_containersMutex);
	_vecObjectsToDelete.push_back(pTerrainObject->GetID());
}


float CTerrainObjectManager::CTerrainObjectManagerImpl::GetWorldRadius() const
{
	return _fWorldScale * 100000000.0f;
}

float CTerrainObjectManager::CTerrainObjectManagerImpl::GetMinCellSize() const
{
	return _fWorldScale * 100.f;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::CreateObjects()
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

void CTerrainObjectManager::CTerrainObjectManagerImpl::CreateObjectsRecursive(const CTerrainBlockDesc* in_pData)
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

void CTerrainObjectManager::CTerrainObjectManagerImpl::CreateObject(const CTerrainBlockDesc* in_pData)
{
	SHeightfield::SCoordinates coords;

	coords.fMinLattitude = in_pData->GetMinimumLattitude();
	coords.fMaxLattitude = in_pData->GetMaximumLattitude();
	coords.fMinLongitude = in_pData->GetMinimumLongitude();
	coords.fMaxLongitude = in_pData->GetMaximumLongitude();

	STriangulationCoordsInfo coordsInfo;
	_pHeightfieldConverter->ComputeTriangulationCoords(coords, coordsInfo);

	CInternalTerrainObject* pObject = new CInternalTerrainObject(static_cast<C3DBaseManager*>(this), _idCurrentIDForNewObject, in_pData, coordsInfo);

	_vecObjects.push_back(pObject);
	_mapId2Object[_idCurrentIDForNewObject] = pObject;

	_idCurrentIDForNewObject++;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::DestroyObjects()
{
	for (CInternalTerrainObject* pObject : _vecObjects)
	{
		delete pObject;
	}

	_vecObjects.clear();
}
