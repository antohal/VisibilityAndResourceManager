#pragma once

#include "ResourceManagerLink.h"

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
