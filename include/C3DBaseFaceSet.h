#pragma once

class C3DBaseMaterial;

// базовый класс фейс-сета
class C3DBaseFaceSet : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_FACESET; }

	// получить ссылку на материал
	virtual C3DBaseMaterial* GetMaterialRef() = 0;
};
