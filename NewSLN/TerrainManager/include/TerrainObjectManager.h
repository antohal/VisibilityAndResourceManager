#pragma once

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

typedef size_t TerrainObjectID;

class CTerrainObjectCreator
{
public:

	// Создать объект Земли по описанию блока. Вернуть идентификатор
	virtual size_t	CreateTerrainObject(const CTerrainBlockDesc* in_pTerrainBlockDesc) = 0;

	// Удалить объект Земли по идентификатору
	virtual void	DeleteTerrainObject(size_t ID) = 0;

	// Загрузить данные объекта Земли (когда он становится потенциально видимым)
	virtual void	LoadTerrainObjectData(size_t ID) = 0;

	// Выгрузить данные объекта Земли (когда он становится невидимым)
	virtual void	UnloadTerrainObjectData(size_t ID) = 0;
};

class TERRAINOBJECTMANAGER_API CTerrainObjectManager
{
public:

	CTerrainObjectManager();
	~CTerrainObjectManager();

	// инициализация. Параметр - имя дериктории, где лежат данные Земли
	void Init(const wchar_t* in_pcwszPlanetDirectory);

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	// В момент вызова этой функции формируется 4 списка: 
	// объекты, которые нужно создать
	// объекты, которые стали видимыми
	// объекты, которые стали невидимыми
	// объекты, которые нужно удалить
	void Update(float in_fDeltaTime);

	// Получить количество новых объектов
	size_t GetNewObjectsCount() const;

	// Получить идентификатор нового объекта Земли по индексу
	TerrainObjectID	GetNewObjectID(size_t index) const;

	//Получить описание объекта Земли по идентификатору
	const CTerrainBlockDesc*	GetTerrainObjectDesc(TerrainObjectID ID) const;

	// Получить количество новых видимых объектов террейна
	size_t GetNewVisibleObjectsCount() const;

	// получить идентификатор нового видимого объекта террейна по индексу
	TerrainObjectID GetNewVisibleObjectID(size_t index) const;


	size_t GetNewInvisibleObjectsCount() const;

	TerrainObjectID GetNewInvisibleObjectID(size_t index);

	size_t GetObjectsToDeleteCount() const;

	TerrainObjectID GetObjectToDeleteID(size_t index) const;

	// получить указатель на менеджер ресурсов (если необходимо задать параметрам предсказателя видимости значения, отличные от значений по-умолчанию)
	CResourceManager* GetResourceManager();

private:

	class CTerrainObjectManagerImpl;
	CTerrainObjectManagerImpl*	_implementation = nullptr;
};