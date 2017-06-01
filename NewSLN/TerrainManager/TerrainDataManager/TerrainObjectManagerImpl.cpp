#include "TerrainObjectManagerImpl.h"
#include "Log.h"

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

//@{ Список объектов, которые стали видимыми
size_t CTerrainObjectManager::GetNewVisibleObjectsCount() const
{
	return _implementation->GetNewVisibleObjectsCount();
}

TerrainObjectID CTerrainObjectManager::GetNewVisibleObjectID(size_t index) const
{
	return _implementation->GetNewVisibleObjectID(index);
}
//@}

//@{ Список объектов, которые стали невидимыми
size_t CTerrainObjectManager::GetNewInvisibleObjectsCount() const
{
	return _implementation->GetNewInvisibleObjectsCount();
}

TerrainObjectID CTerrainObjectManager::GetNewInvisibleObjectID(size_t index)
{
	return _implementation->GetNewInvisibleObjectID(index);
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//		CTerrainObjectManager::CTerrainObjectManagerImpl
//-------------------------------------------------------------------------------------------------------------------------------------------------------

CTerrainObjectManager::CTerrainObjectManagerImpl::~CTerrainObjectManagerImpl()
{
	if (_pResourceManager)
		delete _pResourceManager;

	if (_pVisibilityManager)
		delete _pVisibilityManager;

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

	LogMessage("Loading planet terrain info");

	_pTerrainDataManager->LoadTerrainDataInfo(in_pcwszPlanetDirectory, &_pPlanetTerrainData);

	_pVisibilityManager = new CVisibilityManager(this, GetWorldRadius(), GetMinCellSize());

	_pResourceManager->AddVisibilityManager(_pVisibilityManager);


	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;
	pTerrainVisibilityManager->Init(this, in_fWorldScale);

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

	_pResourceManager->SetViewProjection(vPos, vDir, vUp, const_cast<D3DMATRIX *>(in_pmProjection));
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::Update(float in_fDeltaTime)
{

	_pResourceManager->Update(in_fDeltaTime);
}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetTerrainObjectDesc(TerrainObjectID ID) const
{
	return nullptr;
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewObjectsCount() const
{
	return size_t();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewObjectID(size_t index) const
{
	return TerrainObjectID();
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewVisibleObjectsCount() const
{
	return size_t();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewVisibleObjectID(size_t index) const
{
	return TerrainObjectID();
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewInvisibleObjectsCount() const
{
	return size_t();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetNewInvisibleObjectID(size_t index)
{
	return TerrainObjectID();
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectsToDeleteCount() const
{
	return size_t();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectToDeleteID(size_t index) const
{
	return TerrainObjectID();
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleObjectsCount() const
{
	return size_t();
}

TerrainObjectID CTerrainObjectManager::CTerrainObjectManagerImpl::GetVisibleObjectID(size_t index) const
{
	return TerrainObjectID();
}

CResourceManager * CTerrainObjectManager::CTerrainObjectManagerImpl::GetResourceManager()
{
	return nullptr;
}

size_t CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectsCount() const
{

}

C3DBaseObject*	CTerrainObjectManager::CTerrainObjectManagerImpl::GetObjectByIndex(size_t id) const
{

}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetTerrainDataForObject(C3DBaseObject * pObject) const
{
	return nullptr;
}

const CTerrainBlockDesc * CTerrainObjectManager::CTerrainObjectManagerImpl::GetRootTerrainData() const
{
	return nullptr;
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::RequestLoadResource(C3DBaseResource *)
{
}

void CTerrainObjectManager::CTerrainObjectManagerImpl::RequestUnloadResource(C3DBaseResource *)
{
}


float CTerrainObjectManager::CTerrainObjectManagerImpl::GetWorldRadius() const
{
	return 20000000.f;
}

float CTerrainObjectManager::CTerrainObjectManagerImpl::GetMinCellSize() const
{
	return 1000.f;
}
