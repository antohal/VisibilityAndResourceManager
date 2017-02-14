#pragma once

#include <string>

#include "ResourceManagerLink.h"

class CVisibilityManager;
class C3DBaseObjectManager;
class C3DBaseTexture;
class CDirect2DTextBlock;

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

	// Установить множитель скорости перемещения камеры, с учетом которого
	// баундбокс выборки загружаемых объектов расширяется в сторону движения камеры
	// [настроечный параметр, по умолчанию = 10]
	// (например, при скорости движения 10 м/с, баундбокс расширяется на 100 м)
	void SetSpeedPotentialVisibilityMultiplier(float speedMultiplier);

	// Установить множитель угловой скорости вращения камеры, с учетом
	// которого баундбокс выборки загружаемых объектов поворачивается
	// в сторону вращения камеры
	// [настроечный параметр, по умолчанию = 5]
	// (например при угловой скорости вращения 10 град/с, баунд бокс поворачивается на 50 град)
	void SetRotationRatePotentialVisibilityMultiplier(float rateMultiplier);

	// добавить VisibilityManager в обработку. Необходимо чтобы в этот момент in_pVisibilityManager был полностью инициализирован
	// [вызывается во время инициализации]
	void AddVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	// Обновить состояние ресурсов 
	// [можно вызывать в отдельном потоке, не привязываясь к циклу рендера]
	void Update(float deltaTime);

	// Получить текущий приоритет текстуры при отрисовке
	float GetTexturePriority(C3DBaseTexture*);

	// Включить лог в файл [параметр по умолчанию TRUE]
	void EnableLeg(bool enable = true);

	// Выключить лог в файл
	void DisableLog();

	// Включить рендеринг отладочной информации в текстовый блок
	void EnableDebugTextRender(CDirect2DTextBlock*);

	// Выключить рендеринг отладочной информации в текстовый блок
	void DisableDebugTextRender();

private:

	struct SResourceManagerPrivate;
	SResourceManagerPrivate* _private;

	friend class C3DBaseResource;
	friend class C3DBaseObject;
};
