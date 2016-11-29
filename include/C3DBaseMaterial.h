#pragma once

class C3DBaseTechnique;
class C3DBaseTexture;

// базовый материал
class C3DBaseMaterial : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// Функция используется в процессе рендеринга [для взаимодействия процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*) = 0;

	// Получить набор техник
	virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const = 0;

	// получить список текстур
	virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const = 0;
};
