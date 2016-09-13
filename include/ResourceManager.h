#pragma once

#include <string>

#include "ResourceManagerLink.h"

class CVisibilityManager;
class C3DBaseObjectManager;
class C3DBaseTexture;

// ����� ��������� ��������.
// ������ ������� ���� ������ �������������� � ������������� ����������,
// ������� ��� ��������� ��������� �� ���� ���������� ������������ ������� GetResourceManager()

class RESOURCEMANAGER_API CResourceManager
{
public:

	CResourceManager();
	~CResourceManager();

	// ����������������, ������ MeshTree
	void Init(C3DBaseObjectManager*);

	// ������� �����, ������ �������, ������ ���� ��������� (����� ������� ��������� ������ ������ ����)
	// ��� ���� VisibilityManager ���������� �� ������ 
	// [����������� ��������, �� ��������� = 25�]
	void SetInvisibleUnloadTime(float time);

	// �������� VisibilityManager � ��������� 
	// [���������� �� ����� �������������]
	void AddVisibilityManager(CVisibilityManager*);

	// �������� ��������� �������� 
	// [����� �������� � ��������� ������, �� ������������ � ����� �������]
	void Update(float deltaTime);

	// �������� ������� ��������� �������� ��� ���������
	float GetTexturePriority(C3DBaseTexture*);

	// �������� ������� ��������� ��������� �������� � ����
	void DumpToFile(const std::wstring& fileName);

private:

	struct SResourceManagerPrivate;
	SResourceManagerPrivate* _private;

	friend class C3DBaseResource;
	friend class C3DBaseObject;
};
