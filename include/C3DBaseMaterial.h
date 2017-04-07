#pragma once

#include "C3DBaseTechnique.h"
#include "C3DBaseTexture.h"

class C3DBaseFaceSet;

// базовый материал
class C3DBaseMaterial : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// ‘ункци€ используетс€ в процессе рендеринга [дл€ взаимодействи€ процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) = 0;

	// ѕолучить набор техник
	//virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const = 0;
	virtual size_t	GetTechniquesCount() const = 0;
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const = 0;

	// получить список текстур
	//virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const = 0;
	virtual size_t	GetTexturesCount() const = 0;
	virtual C3DBaseTexture*	GetTextureById(size_t id) const = 0;

	//@{ ѕереопределени€ от C3DBaseResource (их переопредел€ть у пользовател€ не об€зательно)
	virtual size_t GetChildResourceCount() const override final { return GetTechniquesCount() + GetTexturesCount(); }
	
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final
	{ 
		if (id < GetTechniquesCount())
			return GetTechniqueById(id);
		else if (id < GetTechniquesCount() + GetTexturesCount())
			return GetTextureById(id - GetTechniquesCount());

		return nullptr;
	}
	//@}
};
