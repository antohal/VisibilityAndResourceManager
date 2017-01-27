#pragma once

// базовый класс менеджера объектов
class C3DBaseObjectManager : public C3DBaseManager
{
public:

	// получить список объектов
	virtual size_t GetObjectsCount() const = 0;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const = 0;
};
