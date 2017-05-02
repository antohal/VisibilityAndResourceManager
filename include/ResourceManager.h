#pragma once

#include <string>

#include "ResourceManagerLink.h"

//@{ предопределения

class C3DBaseObjectManager;
class C3DBaseTexture;

// Из VisibilityManager.h
struct Vector3;
class CVisibilityManager;

// из D2DTextRenderer
class CDirect2DTextBlock;
struct _D3DMATRIX;
typedef struct _D3DMATRIX D3DMATRIX;
//@}

// Класс менеджера ресурсов.
// Объект данного типа должен присутствовать в единственнном экземпляре,
// поэтому для получения указателя на него необходимо использовать функцию GetResourceManager()

class RESOURCEMANAGER_API CResourceManager
{
public:

	CResourceManager();
	~CResourceManager();

	// Установить отладочные параметры: SetPredictionFOV(45, 45), SetInvisibleUnloadTime(0)
	void SetupDebugParameters();

	// указать время, спустя которое, ресурс став невидимым (когда счетчик видимости станет равным нулю)
	// для всех VisibilityManager выгрузится из памяти 
	// [настроечный параметр, по умолчанию = 10с]
	void SetInvisibleUnloadTime(float time);

	// Установить время предсказания линейного движения камеры
	// [настроечный параметр, по умолчанию = 10]
	// (например, при скорости движения 10 м/с, предсказывающий фруструм перемещается на 100 м в сторону движения камеры)
	void SetSpeedVisibilityPredictionTime(float predictionTime);

	//  Установить время предсказания углового движения камеры
	// [настроечный параметр, по умолчанию = 10]
	// (например при угловой скорости вращения 10 град/с, предсказывающий фруструм поворачивается на 100 град)
	void SetRotationRateVisibilityPredictionTime(float rateMultiplier);

	// Установить интервал усреднения угловой скорости камеры [по умолчанию 0.25 с]
	void SetRotationRateAverageInterval(float intervalSec);

	// Установить область видимости для предсказания видимости
	// По умолчанию берутся значения из связанного VisibilityManager
	void SetPredictionFOV(float horizontalFovDeg, float verticalFovDeg);

	// Установить параметры камеры на текущем кадре
	// Если не вызывать эту функцию, то параметры камеры при прогнозировании видимости ресурсов будут браться из
	// VisibilityManager-ов, привязанных к данному ResourceManager.
	void SetViewProjection(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);

	// Установить параметры камеры на текущем кадре [функция альтернативная вышепредставленной]
	void SetCamera(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane);

	// добавить VisibilityManager в обработку. Необходимо чтобы в этот момент in_pVisibilityManager был полностью инициализирован
	// [вызывается один раз во время инициализации]
	void AddVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	void RemoveVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	// Обновить состояние ресурсов 
	// [можно вызывать в отдельном потоке, не привязываясь к циклу рендера, но в одном потоке с привязанными VisibilityManager]
	void Update(float deltaTime);

	// Получить текущий приоритет текстуры при отрисовке
	float GetTexturePriority(C3DBaseTexture*);

	// Включить лог в файл [параметр по умолчанию TRUE]
	void EnableLog(bool enable = true);

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
