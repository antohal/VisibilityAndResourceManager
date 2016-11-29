#pragma once

// Тип текстуры
enum ETextureType
{
	TEXTURE_2D = 0,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_VOLUME,
	TEXTURE_TYPE_FX,

	TEXTURE_TYPE_COUNT
};

// базовый класс текстуры
class C3DBaseTexture : public C3DBaseResource
{
public:

	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TEXTURE; }

	// вернуть тип текстуры
	virtual ETextureType	GetTextureType() const = 0;
};
