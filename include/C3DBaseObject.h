#pragma once

#include "ResourceManagerLink.h"

#include "C3DBaseMesh.h"

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

	// Функция возврящает количество мешей данного объекта
	virtual size_t	GetMeshesCount() const = 0;

	// Функция возвращает конкретный меш объекта по его идентификатору
	virtual C3DBaseMesh*	GetMeshById(size_t id) const = 0;

	// получить список фейссетов
	virtual size_t	GetFaceSetsCount() const = 0;
	virtual C3DBaseFaceSet*	GetFaceSetById(size_t id) const = 0;

	// Переопределения от C3DBaseResource (их переопределять у пользователя не обязательно)
	virtual size_t GetChildResourceCount() const override final { return GetMeshesCount() + GetFaceSetsCount(); }
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final { 
		if (id < GetMeshesCount()) 
			return GetMeshById(id);

		if (id < GetMeshesCount() + GetFaceSetsCount())
			return GetFaceSetById(id - GetMeshesCount());

		return nullptr;
	}


protected:

	// Эта функция внутренняя, вызывается внутри VisibilityManager в момент, когда объект становится потенциально видимым.
	// Рекурсивно обновляет счетчики видимости всех дочерних ресурсов
	// Однако, пользователь может переопределить её в дочернем классе, если хочет обрабатывать данное событие.
	// При этом обязательно вызвать C3DBaseObject::SetPotentiallyVisible в начале переопределенной функции.
	virtual void SetPotentiallyVisible();

	friend class CVisibilityManager;
};
