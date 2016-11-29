#pragma once

#include "ResourceManagerLink.h"

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

	// Эта функция внутренняя, вызывается внутри VisibilityManager в момент, когда объект становится потенциально видимым.
	// Рекурсивно обновляет счетчики видимости всех дочерних ресурсов
	// Однако, пользователь может переопределить её в дочернем классе, если хочет обрабатывать данное событие.
	// При этом обязательно вызвать C3DBaseObject::SetPotentiallyVisible в начале переопределенной функции.
	virtual void SetPotentiallyVisible();

	friend class CVisibilityManager;
};
