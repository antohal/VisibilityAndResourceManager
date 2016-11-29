#pragma once

class C3DBaseMaterial;

// ������� ����� ����-����
class C3DBaseFaceSet : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_FACESET; }

	// �������� ������ �� ��������
	virtual C3DBaseMaterial* GetMaterialRef() = 0;
};
