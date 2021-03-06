﻿#pragma once

#include <d3d11.h>

#include <string>

#include "TerrainDataManager.h"
#include "TerrainShaderParams.h"

#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINOBJECTMANAGER_API __declspec(dllimport)
#else
#define TERRAINOBJECTMANAGER_API __declspec(dllexport)
#endif

class CTerrainBlockDesc;

struct D3DXVECTOR3;
struct _D3DMATRIX;
typedef struct _D3DMATRIX D3DMATRIX;

class CResourceManager;
class HeightfieldConverter;

struct STriangulation;

typedef size_t TerrainObjectID;
#define INVALID_TERRAIN_OBJECT_ID (size_t)(-1)

struct TerrainManagerConfig
{
	float	MaxLodHeight;
	float	LodCoeff;

};

class TERRAINOBJECTMANAGER_API CTerrainManager
{
public:

	CTerrainManager();
	~CTerrainManager();

	// инициализация. Параметры:
	// in_pD3DDevice11, in_pDeviceContext - объекты Direct3D 11
	// in_pcwszPlanetDirectory - имя дериктории, где лежат данные Земли
	// in_fWorldScale - коэффициент масштаба мира
	// in_fHeightScale - коэффициент масштаба высоты
	//void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
	//	float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff);

	// Инициализировать из файла-описания, согласно заданному формату
	void InitFromDatabaseInfo(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize, bool in_bCalculateAdjacency);

	// Установить триангулятор
	void SetHeightfieldConverter(HeightfieldConverter*);

	// Установить степень сжатия текстур высоты (и соответственно результирующей триангуляции) Может принимать значения степени двойки - 1, 2, 4, 8, 16, ...
	void SetHeightfieldCompressionRatio(unsigned int ratio);

	// Инициализация со случайной генерацией планеты до уровня глубины depth, с разбиением по долготе и широте N и M соответственно. 
	// Текстуры и карты высот беруться случайным образом из [in_pcwszPlanetDirectory]/Textures и /HeightMaps
	//void InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
	//	unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize);

	// Установить положение камеры и матрицу проекции
	void SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	// Рассчитать триангуляции [Функция может вызываться в другом потоке, она дергает ComputeShader, поэтому вызов должен находится вне пределов Present]
	bool UpdateTriangulations();

	const wchar_t*	GetRootDirectory() const;

	// получить имя текстуры для данного объекта
	const wchar_t*	GetTextureFileName(TerrainObjectID ID) const;

	// получить имя карты высот для данного объекта
	const wchar_t*	GetHeightmapFileName(TerrainObjectID ID) const;

	// Проверить - готовы ли данные для объекта
	bool IsObjectDataReady(TerrainObjectID ID) const;

	// Включить/выключить рассчет нормалей по границам.
	// Если включен - то для триангуляции объекта требуются карты нормалей граничных блоков
	// Если выключен - то не требуются 
	// (по умолчанию - включен)
	void SetBorderNormals(bool);

	//Получить описание объекта Земли по идентификатору
	void GetTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams* out_pParams) const;

	// Получить триангуляцию по объекту Земли. Эту функцию обязательно вызывать перед установкой вершин и индексов.
	void GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation** out_ppTriangulation) const;

	// Получить соседей данного блока [возвращаются 8 соседних блоков, начиная с северного по часовой стрелке. Если сосед отсутствует - возвращает -1]
	void GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

	// Получить родительский объект
	TerrainObjectID GetTerrainObjectParent(TerrainObjectID ID) const;

	// Получить количество готовых триангуляций
	size_t GetTriangulationsCount() const;

	// Получить количество готовых карт высот
	size_t GetHeightfieldsCount() const;

	// Получить количество потенциально видимых объектов
	size_t GetPotentiallyVisibleObjectsCount() const;

	//@{ Список объектов для которых нужно загрузить карту высот
	//size_t GetNewHeightmapsCount() const;
	//TerrainObjectID GetNewHeightmapObjectID(size_t index) const;
	//@}

	//@{ Список карт высот, которые ожидают вызова команды SetHeightmapReady
	//size_t GetAwaitingHeightmapsCount() const;
	//TerrainObjectID GetAwaitingHeightmapObjectID(size_t index) const;
	//@}

	// Установить загруженную карту высот
	void SetHeightmapReady(TerrainObjectID ID, ID3D11ShaderResourceView* in_pLoadedHeightmap);

	//@{ Список новых объектов, которые нужно создать (могут стать видимыми), для них необходимо загрузить текстуры
	size_t GetNewObjectsCount() const;
	TerrainObjectID	GetNewObjectID(size_t index) const;
	//@}

	//@{ Список объектов, которые нужно удалить (выпали из списка потенциально видимых)
	size_t GetObjectsToDeleteCount() const;
	TerrainObjectID GetObjectToDeleteID(size_t index) const;
	//@}

	//@{ Список текущих видимых объектов (обязательно загружены)
	size_t GetVisibleObjectsCount() const;
	TerrainObjectID GetVisibleObjectID(size_t index) const;
	//@}

	//@{ набор неготовых объектов во фрустуме
	size_t GetNotReadyObjectsInFrustumCount() const;
	TerrainObjectID GetNotReadyObjectInFrustumID(size_t index) const;
	//@}

	// количество баунд боксов находящихся в очереди на рассчет
	size_t GetBoundBoxToBeCalculatedCount() const;

	// Получить координаты центра объекта террейна (с учетом коэффициента масштаба)
	void GetTerrainObjectCenter(TerrainObjectID ID, D3DXVECTOR3* out_pvCenter) const;

	// Получить проекцию на объект Земли. Возвращает true, если in_pvPosFrom лежит над блоком Земли
	bool GetTerrainObjectProjection(TerrainObjectID ID, const D3DXVECTOR3* in_pvPosFrom, D3DXVECTOR3* out_pvProjection, D3DXVECTOR3* out_pvNormal) const;

	// Получить ближайшую точку объекта Земли. Возвращает true, если in_pvPosFrom лежит над блоком Земли
	bool GetTerrainObjectClosestPoint(TerrainObjectID ID, const D3DXVECTOR3* in_pvPosFrom, D3DXVECTOR3* out_pvClosestPoint, D3DXVECTOR3* out_pvNormal) const;

	// Получить узловые точки баунд бокса
	void GetTerrainObjectBoundBoxCorners(TerrainObjectID ID, D3DXVECTOR3 out_pvCorners[8]) const;

	//@{ Установить признак того, что данные для объекта готовы, и его можно делать видимым
	void SetTextureReady(TerrainObjectID ID);
	//@}

	// Проверить - готова ли триангуляция для объекта Земли
	bool IsTriangulationReady(TerrainObjectID ID) const;


	//@{ Ожидать пока для объекта будут готовы данные, перед тем как делать его видимым [по умолчанию - включено]
	void SetAwaitVisibleForDataReady(bool in_bAwait);
	//@}

	// Установить точность алгоритма расчета минимального расстояния, влияет на скорость.
	// Параметр - размер квадратного субблока в буфере вершин, по которым идет перебор.
	// По умолчанию = 1 - максимальная точность
	// При увеличении - точность уменьшается, скорость увеличивается.
	void SetMinDistAlgorithmAccuracy(unsigned int in_uiAccuracy);

	//// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
	//CResourceManager* GetResourceManager();



	//@{ Функции установки линейки расстояний лодов

	// Установить линейку расстояний для NLods лодов
	void SetLodDistancesKM(double* aLodDistances, size_t NLods);

	// Считать линейку расстояний для NLods лодов
	void GetLodDistancesKM(double* aLodDistances, size_t NLods);

	// Рассчитать автоматически линейку расстояний исходя из максимального количества пикселей на тексель
	// (учитываются: FOV камеры, разрешение экрана, размер текстур лодов, линейные размеры соответствующих блоков Земли)
	void CalculateLodDistances(float in_fMaxPixelsPerTexel, unsigned int in_uiScreenResolutionX, unsigned int in_uiScreenResolutionY);

	// Установить дальность видимости (в метрах) самого детального лода, если камера находится у поверхности Земли
	// 
	void SetLastLodDistanceOnSurface(double distanceMeters);

	//@}



	//@{ Функции получения параметров для шейдеров
	
	// заполнить структуру с глобальными шейдерными параметрами
	void FillGlobalShaderParams(SGlobalTerrainShaderParams* out_pGlobalShaderParams);

	// заполнить структуру с параметрами для указанного блока
	void FillTerrainBlockShaderParams(TerrainObjectID ID, STerrainBlockShaderParams* out_pTerrainBlockShaderParams);

	//@}

private:

	class CTerrainManagerImpl;
	CTerrainManagerImpl*	_implementation = nullptr;
};
