#pragma once

#include "C3DBaseFaceSet.h"

// базовый класс меша
class C3DBaseMesh : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// ѕолучить список ‘ейс-—етов, используемых в меше
	//virtual size_t	GetFaceSetsCount() const = 0;
	//virtual C3DBaseFaceSet*	GetFaceSetById(size_t id) const = 0;

	// ѕереопределени€ от C3DBaseResource (их переопредел€ть у пользовател€ не об€зательно)
	virtual size_t GetChildResourceCount() const override final { return 0; }
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final { return nullptr; }

};
