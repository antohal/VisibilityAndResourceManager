#pragma once


// ������� �������� ����������� ��������
class C3DBaseManager
{
public:

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*) = 0;

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*) = 0;
};
