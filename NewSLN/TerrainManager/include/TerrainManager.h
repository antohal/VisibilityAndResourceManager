#pragma once

#include <d3d11.h>

#include "TerrainDataManager.h"

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
	void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
		float in_fWorldScale, float in_fWorldSize, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff);

	// Инициализировать из файла-описания, согласно заданному формату
	void InitFromDatabaseInfo(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszFileName, unsigned int in_uiMaxDepth, float in_fWorldScale, float in_fWorldSize);

	// Установить триангулятор
	void SetHeightfieldConverter(HeightfieldConverter*);

	// Установить степень сжатия текстур высоты (и соответственно результирующей триангуляции) Может принимать значения степени двойки - 1, 2, 4, 8, 16, ...
	void SetHeightfieldCompressionRatio(unsigned int ratio);

	// Инициализация со случайной генерацией планеты до уровня глубины depth, с разбиением по долготе и широте N и M соответственно. 
	// Текстуры и карты высот беруться случайным образом из [in_pcwszPlanetDirectory]/Textures и /HeightMaps
	void InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, 
		unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fWorldSize);

	// Установить положение камеры и матрицу проекции
	void SetViewProjection(const D3DXVECTOR3* in_vPos, const D3DXVECTOR3* in_vDir, const D3DXVECTOR3* in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	// Рассчитать триангуляции [Функция может вызываться в другом потоке, она дергает ComputeShader, поэтому вызов должен находится вне пределов Present]
	bool UpdateTriangulations();

	// получить имя текстуры для данного объекта
	const wchar_t*	GetTextureFileName(TerrainObjectID ID) const;

	// получить имя карты высот для данного объекта
	const wchar_t*	GetHeightmapFileName(TerrainObjectID ID) const;

	//Получить описание объекта Земли по идентификатору
	const STerrainBlockParams*	GetTerrainObjectParams(TerrainObjectID ID) const;

	// Получить триангуляцию по объекту Земли. Эту функцию обязательно вызывать перед установкой вершин и индексов.
	void GetTerrainObjectTriangulation(TerrainObjectID ID, STriangulation** out_ppTriangulation) const;

	// Получить соседей данного блока [возвращаются 8 соседних блоков, начиная с северного по часовой стрелке. Если сосед отсутствует - возвращает -1]
	void GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

	// Получить количество готовых триангуляций
	size_t GetTriangulationsCount() const;

	// Получить количество готовых карт высот
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

	//@{ Установить признак того, что данные для объекта готовы, и его можно делать видимым
	void SetDataReady(TerrainObjectID ID);
	//@}

	// Проверить - готова ли триангуляция для объекта Земли
	bool IsTriangulationReady(TerrainObjectID ID) const;


	//@{ Ожидать пока для объекта будут готовы данные, перед тем как делать его видимым [по умолчанию - выключено!]
	void SetAwaitVisibleForDataReady(bool in_bAwait);
	//@}

	// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
	CResourceManager* GetResourceManager();

private:

	class CTerrainManagerImpl;
	CTerrainManagerImpl*	_implementation = nullptr;
};
