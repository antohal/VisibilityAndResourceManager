#pragma once

#include "ResourceManagerLink.h"

#include "C3DBaseMesh.h"

// ������� ����� ������� � �����
class RESOURCEMANAGER_API C3DBaseObject : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_OBJECT; }

	// ��� 3D ������� ������ ����� ���������� �����-����. ������, ���� ������ - �����, � �� ���, ��
	// ����� ������ ���������� �������� � out_vBBMin � out_vBBMax.
	virtual void GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) = 0;

	// �������� ������� �������������
	virtual D3DXMATRIX* GetWorldTransform() = 0;

	// ������� ������ ����������: ��������-�� �������� ������� ������� �� ������
	virtual bool IsMinimalSizeCheckEnabled() const = 0;

	// ������� ���������� ���������� ����� ������� �������
	virtual size_t	GetMeshesCount() const = 0;

	// ������� ���������� ���������� ��� ������� �� ��� ��������������
	virtual C3DBaseMesh*	GetMeshById(size_t id) const = 0;

	// �������� ������ ���������
	virtual size_t	GetFaceSetsCount() const = 0;
	virtual C3DBaseFaceSet*	GetFaceSetById(size_t id) const = 0;

	// ��������������� �� C3DBaseResource (�� �������������� � ������������ �� �����������)
	virtual size_t GetChildResourceCount() const override final { return GetMeshesCount() + GetFaceSetsCount(); }
	virtual C3DBaseResource* GetChildResourceById(size_t id) const override final { 
		if (id < GetMeshesCount()) 
			return GetMeshById(id);

		if (id < GetMeshesCount() + GetFaceSetsCount())
			return GetFaceSetById(id - GetMeshesCount());

		return nullptr;
	}


protected:

	// ��� ������� ����������, ���������� ������ VisibilityManager � ������, ����� ������ ���������� ������������ �������.
	// ���������� ��������� �������� ��������� ���� �������� ��������
	// ������, ������������ ����� �������������� � � �������� ������, ���� ����� ������������ ������ �������.
	// ��� ���� ����������� ������� C3DBaseObject::SetPotentiallyVisible � ������ ���������������� �������.
	virtual void SetPotentiallyVisible();

	friend class CVisibilityManager;
};
