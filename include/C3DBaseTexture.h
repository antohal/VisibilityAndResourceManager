#pragma once

// ��� ��������
enum ETextureType
{
	TEXTURE_2D = 0,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_VOLUME,
	TEXTURE_TYPE_FX,

	TEXTURE_TYPE_COUNT
};

// ������� ����� ��������
class C3DBaseTexture : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TEXTURE; }

	// ������� ��� ��������
	virtual ETextureType	GetTextureType() const = 0;
};
