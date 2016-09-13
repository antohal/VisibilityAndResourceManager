#pragma once

#include <vector>

#include "ResourceManagerLink.h"

// predeclarations
struct D3DXMATRIX;
struct D3DXVECTOR3;

// Для использования инфраструктуры ResourceManager пользователю библиотеки следует
// унаследовать свои классы графических ресурсов от базовых классов:
// C3DBaseObject, C3DBaseMesh, C3DBaseFaceSet, C3DBaseMaterial, C3DBaseTechnique, C3DBaseTexture.
// А также определить менеджеры, управляющие данными ресурсами и унаследовать их от базовых классов:
// C3DBaseManager - для всех ресурсов, которые могут быть загружены/выгружены, кроме объектов
// C3DBaseObjectManager - для объектов (отдельно, поскольку выборка объектов производится с помощью отдельного механизма)

// Определение типа ресурсов
enum E3DResourceType
{
	C3DRESOURCE_OBJECT,
	C3DRESOURCE_MESH,
	C3DRESOURCE_MATERIAL,
	C3DRESOURCE_FACESET,
	C3DRESOURCE_TECHNIQUE,
	C3DRESOURCE_TEXTURE
};

class C3DBaseManager;

// базовый ресурс
class RESOURCEMANAGER_API C3DBaseResource
{
public:

	// получить тип ресурса
	virtual E3DResourceType	GetType() const = 0;

	// получить количество ссылок видимых рсурсов на данный ресурс
	virtual int GetVisibleRefCount() const { return _visibleRefCount; }

	// получить дочерние ресурсы
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const = 0;

	// получить указатель на менеджер, управляющий данным ресурсом
	// может быть NULL. В таком случае, ресурс не является выгружаемым
	virtual C3DBaseManager*	GetManager() const = 0;

private:

	int	_visibleRefCount = 0;
	friend class CResourceManager;
};

// базовый менеджер графических ресурсов
class C3DBaseManager
{
public:

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*) = 0;

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*) = 0;
};

class C3DBaseMesh;

// базовый класс объекта в сцене
class RESOURCEMANAGER_API C3DBaseObject : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_OBJECT; }

	// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
	// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
	virtual void GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) = 0;

	// Получить матрицу трансформации
	virtual D3DXMATRIX* GetWorldTransform() = 0;

	// Функция должна возвращать: включена-ли проверка размера объекта на экране
	virtual bool IsMinimalSizeCheckEnabled() const = 0;

	// Функция возврящает набор мешей данного объекта
	virtual void	GetMeshes(std::vector<C3DBaseMesh*>& out_vecMeshes) const = 0;

protected:

	// Эта функция внутренняя, вызывается внутри VisibilityManager в момент, когда объект становится видимым.
	// Рекурсивно обновляет счетчики видимости всех дочерних ресурсов
	// Однако, пользователь может переопределить её в дочернем классе, если хочет обрабатывать данное событие.
	// При этом обязательно вызвать C3DBaseResource::SetVisible в начале переопределенной функции.
	virtual void SetVisible();

	friend class CVisibilityManager;
};

// базовый класс менеджера объектов
class C3DBaseObjectManager : public C3DBaseManager
{
public:

	// получить список объектов по заданному баунд-боксу
	virtual void GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects) = 0;
};

class C3DBaseFaceSet;

// базовый класс меша
class C3DBaseMesh : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// Получить список Фейс-Сетов, используемых в меше
	virtual void	GetFaceSets(std::vector<C3DBaseFaceSet*>& out_vecFaceSets) const = 0;
};

class C3DBaseMaterial;

// базовый класс фейс-сета
class C3DBaseFaceSet : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_FACESET; }

	// получить ссылку на материал
	virtual C3DBaseMaterial* GetMaterialRef() = 0;
};

class C3DBaseTechnique;
class C3DBaseTexture;

// базовый материал
class C3DBaseMaterial : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// Функция используется в процессе рендеринга [для взаимодействия процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*) = 0;

	// Получить набор техник
	virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const = 0;

	// получить список текстур
	virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const = 0;
};

// базовая техника
class C3DBaseTechnique : public C3DBaseResource
{
public:
	
	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TECHNIQUE; }

};

enum ETextureType
{
	TEXTURE_2D = 0,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_VOLUME,
	TEXTURE_TYPE_FX,

	TEXTURE_TYPE_COUNT
};


class C3DBaseTexture : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TEXTURE; }

	// вернуть тип текстуры
	virtual ETextureType	GetTextureType() const = 0;
};
