#pragma once

#include <string>

#include "ResourceManagerLink.h"

class CVisibilityManager;
class C3DBaseObjectManager;
class C3DBaseTexture;

// Класс менеджера ресурсов.
// Объект данного типа должен присутствовать в единственнном экземпляре,
// поэтому для получения указателя на него необходимо использовать функцию GetResourceManager()

class RESOURCEMANAGER_API CResourceManager
{
public:

	CResourceManager();
	~CResourceManager();

	// инициализировать, указав MeshTree
	void Init(C3DBaseObjectManager*);

	// указать время, спустя которое, ресурс став невидимым (когда счетчик видимости станет равным нулю)
	// для всех VisibilityManager выгрузится из памяти 
	// [настроечный параметр, по умолчанию = 25с]
	void SetInvisibleUnloadTime(float time);

	// добавить VisibilityManager в обработку 
	// [вызывается во время инициализации]
	void AddVisibilityManager(CVisibilityManager*);

	// Обновить состояние ресурсов 
	// [можно вызывать в отдельном потоке, не привязываясь к циклу рендера]
	void Update(float deltaTime);

	// Получить текущий приоритет текстуры при отрисовке
	float GetTexturePriority(C3DBaseTexture*);

	// Сбросить текущее состояние менеджера ресурсов в файл
	void DumpToFile(const std::wstring& fileName);

private:

	struct SResourceManagerPrivate;
	SResourceManagerPrivate* _private;

	friend class C3DBaseResource;
	friend class C3DBaseObject;
};
