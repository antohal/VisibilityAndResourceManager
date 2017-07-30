#pragma once

#include "TerrainManager.h"
#include "HeightfieldConverter.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "TerrainVisibilityManager.h"

#include <set>
#include <map>
#include <mutex>

class CInternalTerrainObject : public C3DBaseObject
{
public:

	CInternalTerrainObject(C3DBaseManager* in_pOwner, TerrainObjectID ID, const CTerrainBlockDesc* in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo);

	const TerrainObjectID&	GetID() const {
		return _ID;
	}

	const CTerrainBlockDesc*	GetDesc() const {
		return _pBlockDesc;
	}

	void InvalidateData() {
		_bTriangulationsReady = false;
		_bOtherDataReady = false;
	}

	void SetDataReady() {
		_bOtherDataReady = true;
	}

protected:

	// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
	// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
	virtual void							GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) override;

	// Получить матрицу трансформации
	virtual D3DXMATRIX*						GetWorldTransform() override;

	// Функция должна возвращать: включена-ли проверка размера объекта на экране
	virtual bool							IsMinimalSizeCheckEnabled() const override { return false; };

	// Функция возврящает количество мешей данного объекта
	virtual size_t							GetMeshesCount() const override { return 0; }

	// Функция возвращает конкретный меш объекта по его идентификатору
	virtual C3DBaseMesh*					GetMeshById(size_t id) const { return nullptr; }

	// получить список фейссетов
	virtual size_t							GetFaceSetsCount() const { return 0; }
	virtual C3DBaseFaceSet*					GetFaceSetById(size_t id) const { return nullptr; }

	virtual C3DBaseManager*					GetManager() const { return _pOwner; }

	virtual bool							IsDataReady() const {
		return _bTriangulationsReady && _bOtherDataReady;
	}


private:

	TerrainObjectID				_ID = -1;
	const CTerrainBlockDesc*	_pBlockDesc = nullptr;

	C3DBaseManager*				_pOwner = nullptr;

	D3DXMATRIX					_mTransform;
	D3DXVECTOR3					_vBBoxMin;
	D3DXVECTOR3					_vBBoxMax;

	bool						_bTriangulationsReady = false;
	bool						_bOtherDataReady = false;
};


class CTerrainManager::CTerrainManagerImpl : public C3DBaseTerrainObjectManager
{
public:

	~CTerrainManagerImpl();

	// инициализация. Параметр - имя дериктории, где лежат данные Земли
	void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
		float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff);

	void InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
		unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize);

	void SetHeightfieldConverter(HeightfieldConverter*);

	void SetHeightfieldCompressionRatio(unsigned int ratio) {
		_heightfieldCompressionRatio = ratio;
	}

	void SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые стали видимыми
	// объекты, которые стали невидимыми
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	void UpdateTriangulations();

	// получить имя текстуры для данного объекта
	const wchar_t*	GetTextureFileName(TerrainObjectID ID) const;

	// получить имя карты высот для данного объекта
	const wchar_t*	GetHeightmapFileName(TerrainObjectID ID) const;


	//Получить описание объекта Земли по идентификатору
	const STerrainBlockParams*	GetTerrainObjectParams(TerrainObjectID ID) const;

	void GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation** out_ppTriangulation);

	void GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[4]);

	size_t GetTriangulationsCount() const;

	size_t GetHeightfieldsCount() const;

	//@{ Список новых объектов, которые нужно создать (могут стать видимыми)
	size_t GetNewObjectsCount() const;
	TerrainObjectID	GetNewObjectID(size_t index) const;
	//@}


	//@{ Список объектов, которые нужно удалить (выпали из списка потенциально видимых)
	size_t GetObjectsToDeleteCount() const;
	TerrainObjectID GetObjectToDeleteID(size_t index) const;
	//@}

	//@{ Список текущих видимых объектов
	size_t GetVisibleObjectsCount() const;
	TerrainObjectID GetVisibleObjectID(size_t index) const;
	//@}

	void SetDataReady(TerrainObjectID ID);

	void SetAwaitVisibleForDataReady(bool in_bAwait);


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

	void CreateObjects();
	void CreateObjectsRecursive(const CTerrainBlockDesc* in_pData);
	void CreateObject(const CTerrainBlockDesc* in_pData);
	void DestroyObjects();

	void ReleaseTriangulationsAndHeightmaps();

	void ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords);

	//@{ Main objects
	CResourceManager*		_pResourceManager = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;
	CTerrainVisibilityManager* _pTerrainVisibilityManager = nullptr;

	CTerrainBlockDesc*		_pPlanetTerrainData = nullptr;
	CTerrainDataManager*	_pTerrainDataManager = nullptr;

	HeightfieldConverter*	_pHeightfieldConverter = nullptr;
	//@}

	//@{ Vars
	TerrainObjectID			_idCurrentIDForNewObject = 0;
	float					_fWorldScale = 1.f;
	float					_fWorldSize = 10000000.f;
	unsigned int			_heightfieldCompressionRatio = 1;
	//@}

	//@{ Containers
	std::vector<CInternalTerrainObject*>				_vecObjects;
	std::map<TerrainObjectID, CInternalTerrainObject*>	_mapId2Object;
	std::map<const CTerrainBlockDesc*, TerrainObjectID>	_mapDesc2ID;

	mutable std::mutex									_containersMutex;

	//@{ following containers are guarded by mutex (_containersMutex)
	std::vector<TerrainObjectID>						_vecNewObjectIDs;
	std::vector<TerrainObjectID>						_vecObjectsToDelete;

	//@}

	//@}

	struct SObjectTriangulation
	{
		STriangulation	_triangulation;
		float			_timeSinceDead = 0;
		bool			_alive = false;
	};

	struct SObjectHeightfield
	{
		SHeightfield	_heightfield;
		float			_timeSinceLastRequest = 0;
	};

	SHeightfield*		RequestObjectHeightfield(TerrainObjectID ID);

	mutable std::mutex									_triangulationsMutex;

	std::map<TerrainObjectID, SObjectTriangulation>		_mapObjectTriangulations;
	std::map<TerrainObjectID, SObjectHeightfield>		_mapObjectHeightfields;
};
