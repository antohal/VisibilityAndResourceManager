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

class CInternalTerrainObject  : public C3DBaseObject
{
public:

	CInternalTerrainObject(C3DBaseManager* in_pOwner, TerrainObjectID ID, const STerrainBlockParams& in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName);

	const TerrainObjectID&	GetID() const {
		return _ID;
	}

	const STerrainBlockParams*	GetParams() const {
		return &_params;
	}

	void InvalidateData() {
		_bTriangulationsReady = false;
		_bOtherDataReady = false;
	}

	void SetDataReady() {
		_bOtherDataReady = true;
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
		return _bTriangulationsReady && _bOtherDataReady;
	}

	void CalculateReferencePoints(std::vector<vm::Vector3df>& out_vecPoints, std::vector<vm::Vector3df>& out_vecNormals);

	const wchar_t* GetTextureFileName() const {
		return _textureFileName.c_str();
	}

	const wchar_t* GetHeightmapFileName() const {
		return _heightmapFileName.c_str();
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


private:

	TerrainObjectID				_ID = -1;
	//const CTerrainBlockDesc*	_pBlockDesc = nullptr;
	STerrainBlockParams			_params;

	C3DBaseManager*				_pOwner = nullptr;

	STriangulation*				_pTriangulation = nullptr;

	D3DXMATRIX					_mTransform;
	D3DXVECTOR3					_vBBoxMin;
	D3DXVECTOR3					_vBBoxMax;

	bool						_bTriangulationsReady = false;
	bool						_bOtherDataReady = false;

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

	// инициализация. Параметр - имя дериктории, где лежат данные Земли
	void InitFromDatabaseInfo(ID3D11Device * in_pD3DDevice11, ID3D11DeviceContext * in_pDeviceContext, const wchar_t * in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency);

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

	bool UpdateTriangulations();

	// получить имя текстуры для данного объекта
	const wchar_t*	GetTextureFileName(TerrainObjectID ID) const;

	// получить имя карты высот для данного объекта
	const wchar_t*	GetHeightmapFileName(TerrainObjectID ID) const;


	//Получить описание объекта Земли по идентификатору
	const STerrainBlockParams*	GetTerrainObjectParams(TerrainObjectID ID) const;

	void GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation** out_ppTriangulation);

	void GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

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

	// Проверить - готова ли триангуляция для объекта Земли
	bool IsTriangulationReady(TerrainObjectID ID) const;

	//@{ Функции установки линейки расстояний лодов

	// Установить линейку расстояний для NLods лодов
	void SetLodDistancesKM(double* aLodDistances, size_t NLods);

	// Считать линейку расстояний для NLods лодов
	void GetLodDistancesKM(double* aLodDistances, size_t NLods);

	// Рассчитать автоматически линейку расстояний исходя из максимального количества пикселей на тексель
	// (учитываются: FOV камеры, разрешение экрана, размер текстур лодов, линейные размеры соответствующих блоков Земли)
	// После вызова этой функции, на следующем вызове Update произойдет пересчет линейки лодов.
	void CalculateLodDistances(float in_fMaxPixelsPerTexel, unsigned int in_uiScreenResolutionX, unsigned int in_uiScreenResolutionY);

	//@}


	//@{ Функции получения параметров для шейдеров

	// заполнить структуру с глобальными шейдерными параметрами
	void FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams);

	// заполнить структуру с параметрами для указанного блока
	void FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams);

	//@}


private:

	float GetWorldRadius() const;
	float GetMinCellSize() const;

	void CreateObject(TerrainObjectID ID);
	void DestroyObject(TerrainObjectID ID);
	void DestroyObjects();

	void ReleaseTriangulationsAndHeightmaps();

	void ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords, unsigned int nLod);

	void CalculateReadyAndVisibleSet();

	bool CheckPointsInFrustum(const std::vector<vm::Vector3df>& vecPoints) const;

	bool FillTerrainBlockParams(TerrainObjectID ID, STerrainBlockParams& out_Params) const;
	void UpdateTriangulationsAndHeightfieldLifetime();

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
	std::vector<TerrainObjectID>						_vecNotCheckedForTriangulations;
	std::vector<TerrainObjectID>						_vecObjectsToDelete;
	std::vector<TerrainObjectID>						_vecPreliminaryObjectsToDelete;

	std::vector<TerrainObjectID>						_vecReadyVisibleObjects;
	std::set<CInternalTerrainObject*>					_setPreliminaryVisibleObjects;
	//@}

	//@}

	struct SObjectTriangulation
	{
		STriangulation	_triangulation;
		double			_timeSinceDead = 0;
		bool			_alive = false;
	};

	struct SObjectHeightfield
	{
		SHeightfield	_heightfield;
		double			_timeSinceLastRequest = 0;
	};

	SHeightfield*		RequestObjectHeightfield(TerrainObjectID ID);

	mutable std::mutex									_objectTriangulationsMutex;
	std::map<TerrainObjectID, SObjectTriangulation>		_mapObjectTriangulations;

	mutable std::mutex									_objectHeightfieldsMutex;
	std::map<TerrainObjectID, SObjectHeightfield>		_mapObjectHeightfields;

	bool			_bAwaitingVisibleForDataReady = true;

	struct SCameraParams
	{
		vm::Vector3df		vPos = vm::Vector3df(0, 0, 0);
		vm::Vector3df		vDir = vm::Vector3df(1, 0, 0);
		vm::Vector3df		vUp = vm::Vector3df(0, 1, 0);

		D3DMATRIX			mProjection;

		float				fHFovAngleRad, fVFovAngleRad;
		unsigned int		uiScreenResolutionX, uiScreenResolutionY;
	};

	SCameraParams			_cameraParams;

	std::vector<size_t>		_vecLODResolution;
	std::vector<float>		_vecLODDiameter;

	float					_fMaxPixelsPerTexel = 10;
	bool					_bRecalculateLodsDistances = false;

};
