#pragma once

#include "C3DBaseMaterial.h"

// базовый класс фейс-сета
class C3DBaseFaceSet : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_FACESET; }

	// получить ссылку на материал
	virtual C3DBaseMaterial* GetMaterialRef() = 0;

	virtual size_t GetChildResourceCount() const override final
	{
		return 1;
	}

	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final
	{
		if (id == 0)
			return const_cast<C3DBaseFaceSet*>(this)->GetMaterialRef();

		return nullptr;
	}

};
