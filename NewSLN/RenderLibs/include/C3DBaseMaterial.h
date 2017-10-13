#pragma once

#include "ResourceManagerLink.h"

#include "C3DBaseTechnique.h"
#include "C3DBaseTexture.h"

class C3DBaseFaceSet;

// ������� ��������
class RESOURCEMANAGER_API C3DBaseMaterial : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// ������� ������������ � �������� ���������� [��� �������������� �������� ������� � Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) = 0;

	// �������� ����� ������
	virtual size_t	GetTechniquesCount() const = 0;
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const = 0;

	// �������� ������ �������
	virtual size_t	GetTexturesCount() const = 0;
	virtual C3DBaseTexture*	GetTextureById(size_t id) const = 0;

	// �������� ���������� ������� �� ����
	virtual size_t	GetTexturesCountByType(ETextureType) const = 0;

	// �������� �������� �� ���� � ������
	virtual C3DBaseTexture* GetTextureByTypeAndId(ETextureType in_Type, size_t id) const = 0;

	//@{ ��������������� �� C3DBaseResource (�� �������������� � ������������ �� �����������)
	virtual size_t GetChildResourceCount() const override final;
	
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final;
	//@}
};
