#pragma once

class C3DBaseFaceSet;

// ������� ����� ����
class C3DBaseMesh : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// �������� ������ ����-�����, ������������ � ����
	virtual void	GetFaceSets(std::vector<C3DBaseFaceSet*>& out_vecFaceSets) const = 0;
};
