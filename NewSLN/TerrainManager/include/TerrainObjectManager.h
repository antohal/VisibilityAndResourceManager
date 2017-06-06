#pragma once

#include <d3d11.h>

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

typedef size_t TerrainObjectID;

class TERRAINOBJECTMANAGER_API CTerrainObjectManager
{
public:

	CTerrainObjectManager();
	~CTerrainObjectManager();

	// инициализация. Параметры:
	// in_pD3DDevice11, in_pDeviceContext - объекты Direct3D 11
	// in_pcwszPlanetDirectory - имя дериктории, где лежат данные Земли
	// in_fWorldScale - коэффициент масштаба мира
	// in_fHeightScale - коэффициент масштаба высоты
	void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fHeightScale);

	// Инициализация со случайной генерацией планеты до уровня глубины depth, с разбиением по долготе и широте N и M соответственно. 
	// Текстуры и карты высот беруться случайным образом из [in_pcwszPlanetDirectory]/Textures и /HeightMaps
	void InitGenerated(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, unsigned int N, unsigned int M, unsigned int depth, float in_fWorldScale, float in_fHeightScale);

	// Установить положение камеры и матрицу проекции
	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	//Получить описание объекта Земли по идентификатору
	const CTerrainBlockDesc*	GetTerrainObjectDesc(TerrainObjectID ID) const;

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

	// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
	CResourceManager* GetResourceManager();

	// получить конвертер карт высот
	HeightfieldConverter*	GetHeightfieldConverter();

private:

	class CTerrainObjectManagerImpl;
	CTerrainObjectManagerImpl*	_implementation = nullptr;
};
