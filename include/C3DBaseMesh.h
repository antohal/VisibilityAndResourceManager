#pragma once

class C3DBaseFaceSet;

// базовый класс меша
class C3DBaseMesh : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// Получить список Фейс-Сетов, используемых в меше
	virtual void	GetFaceSets(std::vector<C3DBaseFaceSet*>& out_vecFaceSets) const = 0;
};
