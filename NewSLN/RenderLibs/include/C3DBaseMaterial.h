#pragma once

#include "ResourceManagerLink.h"

#include "C3DBaseTechnique.h"
#include "C3DBaseTexture.h"

class C3DBaseFaceSet;

// базовый материал
class RESOURCEMANAGER_API C3DBaseMaterial : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// Функция используется в процессе рендеринга [для взаимодействия процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) = 0;

	// Получить набор техник
	virtual size_t	GetTechniquesCount() const = 0;
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const = 0;

	// получить список текстур
	virtual size_t	GetTexturesCount() const = 0;
	virtual C3DBaseTexture*	GetTextureById(size_t id) const = 0;

	// получить количество текстур по типу
	virtual size_t	GetTexturesCountByType(ETextureType) const = 0;

	// получить текстуру по типу и номеру
	virtual C3DBaseTexture* GetTextureByTypeAndId(ETextureType in_Type, size_t id) const = 0;

	//@{ Переопределения от C3DBaseResource (их переопределять у пользователя не обязательно)
	virtual size_t GetChildResourceCount() const override final;
	
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final;
	//@}
};
