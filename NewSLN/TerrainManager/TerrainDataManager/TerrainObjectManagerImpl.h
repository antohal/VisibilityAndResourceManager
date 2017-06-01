#pragma once

#include "TerrainDataManager.h"
#include "TerrainObjectManager.h"
#include "HeightfieldConverter.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "TerrainVisibilityManager.h"


class CTerrainObjectManager::CTerrainObjectManagerImpl : public C3DBaseTerrainObjectManager
{
public:

	~CTerrainObjectManagerImpl();

	// инициализация. Параметр - имя дериктории, где лежат данные Земли
	void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fHeightScale);

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые стали видимыми
	// объекты, которые стали невидимыми
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	//Получить описание объекта Земли по идентификатору
	const CTerrainBlockDesc*	GetTerrainObjectDesc(TerrainObjectID ID) const;

	//@{ Список новых объектов, которые нужно создать (могут стать видимыми)
	size_t GetNewObjectsCount() const;
	TerrainObjectID	GetNewObjectID(size_t index) const;
	//@}

	//@{ Список объектов, которые стали видимыми
	size_t GetNewVisibleObjectsCount() const;
	TerrainObjectID GetNewVisibleObjectID(size_t index) const;
	//@}

	//@{ Список объектов, которые стали невидимыми
	size_t GetNewInvisibleObjectsCount() const;
	TerrainObjectID GetNewInvisibleObjectID(size_t index);
	//@}

	//@{ Список объектов, которые нужно удалить (выпали из списка потенциально видимых)
	size_t GetObjectsToDeleteCount() const;
	TerrainObjectID GetObjectToDeleteID(size_t index) const;
	//@}

	//@{ Список текущих видимых объектов
	size_t GetVisibleObjectsCount() const;
	TerrainObjectID GetVisibleObjectID(size_t index) const;
	//@}


	// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
	CResourceManager* GetResourceManager();

	//@{ C3DBaseObjectManager
	// получить список объектов
	
	virtual size_t GetObjectsCount() const override;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const override;
	
	//@}

	//@{ C3DBaseTerrainObjectManager

	// получить данные блока по объекту
	virtual const CTerrainBlockDesc* GetTerrainDataForObject(C3DBaseObject* pObject) const override;

	// получить указатель на корневой блок [с корневым узлом не должно быть связано никаких объектов, он служит как хранилище]
	virtual const CTerrainBlockDesc* GetRootTerrainData() const override;

	//@}


	//@{ C3DBaseManager

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*) override;

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*) override;

	//@} C3DBaseManager

private:

	float GetWorldRadius() const;
	float GetMinCellSize() const;

	CResourceManager*		_pResourceManager = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;

	CTerrainBlockDesc*		_pPlanetTerrainData = nullptr;
	CTerrainDataManager*	_pTerrainDataManager = nullptr;

	HeightfieldConverter*	_pHeightfieldConverter = nullptr;

};