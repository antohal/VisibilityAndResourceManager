#pragma once

#include "C3DBaseFaceSet.h"

// ������� ����� ����
class C3DBaseMesh : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// �������� ������ ����-�����, ������������ � ����
	//virtual size_t	GetFaceSetsCount() const = 0;
	//virtual C3DBaseFaceSet*	GetFaceSetById(size_t id) const = 0;

	// ��������������� �� C3DBaseResource (�� �������������� � ������������ �� �����������)
	virtual size_t GetChildResourceCount() const override final { return 0; }
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final { return nullptr; }

};
