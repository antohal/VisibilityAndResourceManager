#pragma once

#include "ResourceManagerLink.h"

// ��� ������������� �������������� ResourceManager ������������ ���������� �������
// ������������ ���� ������ ����������� �������� �� ������� �������:
// C3DBaseObject, C3DBaseMesh, C3DBaseFaceSet, C3DBaseMaterial, C3DBaseTechnique, C3DBaseTexture.
// � ����� ���������� ���������, ����������� ������� ��������� � ������������ �� �� ������� �������:
// C3DBaseManager - ��� ���� ��������, ������� ����� ���� ���������/���������, ����� ��������
// C3DBaseObjectManager - ��� �������� (��������, ��������� ������� �������� ������������ � ������� ���������� ���������)

// ����������� ���� ��������
enum E3DResourceType
{
	C3DRESOURCE_OBJECT,
	C3DRESOURCE_MESH,
	C3DRESOURCE_MATERIAL,
	C3DRESOURCE_FACESET,
	C3DRESOURCE_TECHNIQUE,
	C3DRESOURCE_TEXTURE
};

class C3DBaseManager;

// ������� ������
class RESOURCEMANAGER_API C3DBaseResource
{
public:

	// �������� ��� �������
	virtual E3DResourceType	GetType() const = 0;

	// �������� ���������� ������ ������� ������� �� ������ ������
	virtual int GetVisibleRefCount() const { return _visibleRefCount; }

	// �������� �������� �������
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const = 0;

	// �������� ��������� �� ��������, ����������� ������ ��������
	// ����� ���� NULL. � ����� ������, ������ �� �������� �����������
	virtual C3DBaseManager*	GetManager() const = 0;

private:

	int	_visibleRefCount = 0;
	friend class CResourceManager;
};
