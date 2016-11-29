#pragma once

// ������� ����� ��������� ��������
class C3DBaseObjectManager : public C3DBaseManager
{
public:

	// �������� ������ �������� �� ��������� �����-�����
	virtual void GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects) = 0;
};
