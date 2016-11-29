#pragma once

#include "ResourceManagerLink.h"

class C3DBaseMesh;

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

	// ������� ���������� ����� ����� ������� �������
	virtual void	GetMeshes(std::vector<C3DBaseMesh*>& out_vecMeshes) const = 0;

protected:

	// ��� ������� ����������, ���������� ������ VisibilityManager � ������, ����� ������ ���������� ������������ �������.
	// ���������� ��������� �������� ��������� ���� �������� ��������
	// ������, ������������ ����� �������������� � � �������� ������, ���� ����� ������������ ������ �������.
	// ��� ���� ����������� ������� C3DBaseObject::SetPotentiallyVisible � ������ ���������������� �������.
	virtual void SetPotentiallyVisible();

	friend class CVisibilityManager;
};
